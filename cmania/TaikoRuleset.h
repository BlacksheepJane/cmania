#pragma once
#include <vector>
#include "Animator.h"
#include "TaikoObject.h"
#include "Ruleset.h"
#include "OsuBeatmap.h"
#include "TaikoScoreProcessor.h"
#include <filesystem>
#include <set>
#include "File.h"
#include "BassAudioManager.h"
#include "ConsoleInput.h"
#include "KeyBinds.h"
#include "OsuSample.h"

class TaikoRuleset : public Ruleset<TaikoObject> {
	std::vector<Animator<PowerEasingFunction<1.5>>> KeyHighlight;
	Animator<PowerEasingFunction<4.0>> LastHitResultAnimator{ 255, 0, 400 };
	HitResult LastHitResult = HitResult::None;
	AudioStream bgm;
	double scrollspeed = 0;
	double endtime = 0;
	std::string skin_path;
	OsuBeatmap orig_bmp;
	std::filesystem::path parent_path;
	double offset = 0;
	double first_obj = 1e300;
	double end_obj = -1e300;
	double resume_time = -1e300;
	int keys = 0;
	bool jump_helper = false;
	bool no_hs = false;
	bool wt_mode = false;
	bool tail_hs = false;
	double last_rec = 0;
	double miss_offset = 200;

public:
	TaikoRuleset() {
		RulesetScoreProcessor = new TaikoScoreProcessor();
		KeyHighlight.resize(18, { 180, 0, 150 });
	}
	~TaikoRuleset() {
		delete RulesetScoreProcessor;
	}
	// ͨ�� Ruleset �̳�
	virtual void LoadSettings(BinaryStorage& settings) override {
		scrollspeed = settings["ScrollSpeed"].Get<double>();
		if (scrollspeed <= 0) {
			scrollspeed = 500;
		}
		offset = settings["Offset"].Get<double>();
		no_hs = settings["NoBmpHs"].Get<bool>();

		jump_helper = settings["JumpHelper"].Get<bool>();
		tail_hs = settings["TailHs"].Get<bool>();
		const char* skinpath = settings["SkinPath"].GetString();
		if (skinpath == 0 || !std::filesystem::exists(skinpath)) {
			skinpath = "Samples/Triangles";
		}
		skin_path = skinpath;
		auto& binds = settings["KeyBinds"];
		if (binds.Size < key_binds.size() * sizeof(ConsoleKey)) {
			binds.SetArray(key_binds.data(), key_binds.size());
			settings.Write();
		}
		auto dat = binds.GetArray<ConsoleKey>();
		std::copy(dat, dat + key_binds.size(), key_binds.begin());
	}
	virtual void Load(std::filesystem::path beatmap_path) override {
		auto am = GetBassAudioManager(); // ��ȡBass����

		// ��ȡosu����ĸ�Ŀ¼��Ҳ���������Ŀ¼��
		std::filesystem::path parent = beatmap_path.parent_path();
		parent_path = parent;

		// �򿪲�����osu�����ļ�
		{
			// ��osu�����ļ�
			auto ifs = std::ifstream(beatmap_path);
			if (!ifs.good())
				throw std::invalid_argument("Cannot open file.");

			// ����osu����
			orig_bmp = OsuBeatmap::Parse(ifs);

			RulesetRecord.BeatmapTitle = orig_bmp.Title;
			RulesetRecord.BeatmapVersion = orig_bmp.Version;
			RulesetRecord.BeatmapHash = 0;
		} // RAII ���� ifstream ��Դ

		keys = static_cast<int>(orig_bmp.CircleSize); // ��ȡ����

		// �洢��Ҫ���صĲ���
		std::set<std::string> Samples;

		Samples > AddRangeSet(orig_bmp.HitObjects > Select([](const auto& ho) -> std::string { return ho.CustomSampleFilename; }) > Where([](const auto& str) -> bool { return !str.empty(); }) > Select([&](const auto& str) -> auto { return (parent / str).string(); })) > AddRangeSet(
																																																																				  orig_bmp.StoryboardSamples > Select([&](const auto& item) -> auto { return (parent / item.path).string(); }));

		auto SampleIndex = BuildSampleIndex(parent, 1);		   // ���������������(sampleset==1Ĭ��)
		auto SkinSampleIndex = BuildSampleIndex(skin_path, 0); // ����Ƥ����������(sampleset==0)

		auto selector = [](const AudioSampleMetadata& md) -> auto {
			return md.filename.string();
		}; // linq ��ѯ

		Samples > AddRangeSet(Select(SampleIndex, selector))  // ����������·��
			> AddRangeSet(Select(SkinSampleIndex, selector)); // ���Ƥ������·��

		std::map<std::string, AudioSample> SampleCaches; // ��������

		Samples > ForEach([&](const std::string& path) {
			try {
				auto dat = ReadAllBytes(path);
				SampleCaches[path] = AudioSample(am->loadSample(dat.data(), dat.size()));
			}
			catch (...) {
			}
		});

		// �������
		Beatmap > AddRange(Select(
					  orig_bmp.HitObjects, [&](const OsuBeatmap::HitObject& obj) -> auto {
						  TaikoObject to{};
						  first_obj = std::min(first_obj, obj.StartTime);
						  end_obj = std::max(end_obj, obj.StartTime);
						  to.StartTime = obj.StartTime;
						  auto tp = GetTimingPointTiming(orig_bmp, obj.StartTime);
						  auto bpm = tp.BPM();
						  auto blen = 60.0 / bpm;
						  auto vec = GetTimingPointNonTiming(orig_bmp, obj.StartTime).SpeedMultiplier();
						  auto odd_s = std::abs(orig_bmp.SliderTickRate - 3) < 0.01;
						  bool isKat = HasFlag(obj.SoundType, HitSoundType::Whistle) || HasFlag(obj.SoundType, HitSoundType::Clap);
						  bool isLarge = HasFlag(obj.SoundType, HitSoundType::Finish);
						  auto scoringdist = BASE_SCORING_DISTANCE * orig_bmp.SliderMultiplier * vec;
						  auto velocity = scoringdist / tp.BeatLength;
						  if (HasFlag(obj.Type, HitObjectType::Slider)) {
							  // ������Ҫ���� Slider ��Ҫ������
							  /*
							  ժҪ��wiki:

							  ����BPM �� 125 �����µ����棬����� 1/8 ���� 1/4 ������

							  ��ע�⣬�����в���ʹ�� 1/8 ���ࡣ�������� 1/8 �����·��û�����

							  ҲҪע�⣬��������㱶��Ϊ 3�������� 1/6 ������
							  */
							  auto snap = odd_s ? 6 : (bpm <= 125.0 ? 8 : 4);
							  blen /= snap;
							  auto duration = obj.Length / velocity;
							  auto hits = (int)(duration / blen);
							  to.TotalHits = to.RemainsHits = hits;
							  to.TickTime = blen;
							  to.ObjectType = ModifyFlag(to.ObjectType,isKat ? TaikoObject::Kat : TaikoObject::Don);
							  if (isLarge)
								  to.ObjectType = ModifyFlag(to.ObjectType, TaikoObject::Large);
							  to.EndTime = to.StartTime + duration;
						  }
						  else if (HasFlag(obj.Type, HitObjectType::Spinner)) {
							  auto snap = odd_s ? 6 : (bpm <= 125.0 ? 8 : 4);
							  blen /= snap;
							  auto duration = obj.Length / velocity;
							  auto hits = (int)(duration / blen);
							  to.TotalHits = to.RemainsHits = hits;
							  to.TickTime = blen;
							  to.ObjectType = TaikoObject::Spinner;
							  to.EndTime = to.StartTime + duration;
						  }
						  else if (HasFlag(obj.Type, HitObjectType::Circle)) {
							  to.ObjectType = ModifyFlag(to.ObjectType, isKat ? TaikoObject::Kat : TaikoObject::Don);
							  if (isLarge)
								  to.ObjectType = ModifyFlag(to.ObjectType, TaikoObject::Large);
						  }
						  return to;
					  }));

		// ����bgm
		{
			auto dat = ReadAllBytes((parent / orig_bmp.AudioFilename).string());
			bgm = AudioStream(am->load(dat.data(), dat.size()));
		}

		if (RulesetInputHandler == 0)
			throw std::invalid_argument("RulesetInputHandler mustn't be nullptr.");

		auto binds = Select(
			GetKeyBinds(keys), [](const auto& val) -> auto { return (int)val; })
						 .ToList<int>();
		RulesetInputHandler->SetBinds(binds);

		RulesetRecord.Mods = Mods;
		RulesetRecord.RatingGraph.resize(((end_obj - first_obj) + 11000) / 100);

		RulesetScoreProcessor->RulesetRecord = &RulesetRecord;
		RulesetScoreProcessor->SetDifficulty(orig_bmp.OverallDifficulty);
		RulesetScoreProcessor->SetMods(Mods);

		RulesetScoreProcessor->ApplyBeatmap(1 * GetPlaybackRate(Mods));

		miss_offset = GetHitRanges(orig_bmp.OverallDifficulty)[HitResult::Meh];

		Clock.SetRate(GetPlaybackRate(Mods));
		Clock.Offset(std::min(first_obj - 5000, -3000.0)); // �������ʱ��׼������
		Clock.Start();									   // ��ʼHpet��ʱ��

		RulesetInputHandler->SetClockSource(Clock);

		GameStarted = true;
	}
	virtual void Update() override {
		if (GameEnded)
			return;
		auto time = Clock.Elapsed();

		if (bgm != 0 && (time > bgm->getDuration() * 1000 + 3000 || time > end_obj + 3000)) {
			GameEnded = true;
			Clock.Stop();
			return;
		}

		if (time > first_obj) {
			RulesetRecord.RatingGraph[(time - first_obj) / 100] = RulesetScoreProcessor->Rating;
		}

		if (time < resume_time || !Clock.Running())
			return;

		if (bgm != 0 && time > -30 && time < bgm->getDuration() * 1000 - 3000) {
			if (!bgm->isPlaying()) {
				if (!bgm->isPaused()) // ��������һЩС��������Ƶ��Clock����ͬ��
				{
					Clock.Stop();
					bgm->setPlaybackRate(Clock.ClockRate());
					bgm->play();
					while (bgm->getCurrent() < 0.003) {
					}
					Clock.Reset();
					Clock.Offset(bgm->getCurrent() * 1000 + offset);
					Clock.Start();
				}
				else {
					bgm->pause(false);
				}
			}
			else {
				auto err = time - bgm->getCurrent() * 1000;
				// auto str = std::to_string(err);
				// DbgOutput(str.c_str());
				// DbgOutput("\n");
				if (std::abs(err) > 150) // bgm get too far away from hpet timer
				{
					bgm->setCurrent(time / 1000); // seek earlier.

					// We doesn't really care about what will happen on a low end machine right?
				}
			}
		}
		while (true) {
			auto evt = RulesetInputHandler->PollEvent();
			if (!evt.has_value())
				break;
			auto& e = *evt;
			RulesetRecord.Events.push_back(e);
		}
	}
	double CalcFlashlight(OsuMods mods, double ratio) {
		if (ratio > 1 && ratio < 0)
			return 1;
		if (HasFlag(mods, OsuMods::Hidden)) {
			if (ratio < 0.4) {
				return pow(ratio / 0.4, 2);
			}
		}
		if (HasFlag(mods, OsuMods::FadeOut)) {
			if (ratio > 0.6) {
				return pow((1 - ratio) / 0.4, 2);
			}
		}
		return 1;
	}
	virtual void Render(GameBuffer& buffer) override {
		auto e_ms = Clock.Elapsed();
	}

	// ͨ�� Ruleset �̳�
	virtual void Pause() override {
		for (auto& light : KeyHighlight) {
			light.Reset();
		}
		bgm->pause(true);
		Clock.Stop();
	}
	virtual void Resume() override {
		resume_time = Clock.Elapsed();
		Clock.Offset(-3000);
		Clock.Start();
	}
	virtual void Skip() override {
		if (first_obj - 3000 > 1000 && Clock.Elapsed() < first_obj - 3000) {
			Clock.Reset();
			Clock.Offset(first_obj - 3000);
		}
	}

	// ͨ�� Ruleset �̳�
	virtual double GetCurrentTime() override {
		return Clock.Elapsed() - first_obj;
	}
	virtual double GetDuration() override {
		return end_obj - first_obj;
	}

	// ͨ�� Ruleset �̳�
	virtual std::string GetBgPath() override {
		return (parent_path / orig_bmp.Background).string();
	}

	// ͨ�� Ruleset �̳�
	virtual Record GetAutoplayRecord() override {
		Record record{};
		record.PlayerName = "Autoplay";
		record.Events.clear();
		return record;
	}
};