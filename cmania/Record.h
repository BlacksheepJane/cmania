﻿#pragma once
#include "OsuMods.h"
#include <vector>
#include <map>
#include <ostream>
#include "OsuStatic.h"
#include "InputEvent.h"
#include "SettingStorage.h"
/// <summary>
/// 表示一个成绩的录像
/// </summary>
class Record {
public:
	OsuMods Mods{};
	double Score = 0;
	double Accuracy = 0;
	unsigned int MaxCombo = 0;
	unsigned int BeatmapMaxCombo = 0;
	// 与谱面对比用的哈希
	unsigned int BeatmapHash = 0;
	double Rating = 0;
	double Mean = 0;
	double Error = 0;
	std::string BeatmapTitle;
	std::string BeatmapVersion;
	std::string PlayerName;
	// 100ms进行一次采样
	std::vector<double> HealthGraph;
	// 100ms进行一次采样
	std::vector<double> RatingGraph;
	std::map<HitResult, int> ResultCounter;
	std::vector<InputEvent> Events;
	void Write(std::ostream& os) const {
		Binary::Write(os, Mods);
		Binary::Write(os, Score);
		Binary::Write(os, Accuracy);
		Binary::Write(os, MaxCombo);
		Binary::Write(os, BeatmapMaxCombo);
		Binary::Write(os, Rating);
		Binary::Write(os, Mean);
		Binary::Write(os, Error);
		Binary::Write(os, BeatmapHash);
		Binary::Write(os, BeatmapTitle);
		Binary::Write(os, BeatmapVersion);
		Binary::Write(os, PlayerName);
		Binary::Write(os, HealthGraph);
		Binary::Write(os, RatingGraph);
		Binary::Write(os, ResultCounter);
		Binary::Write(os, Events);
	}
	void Read(std::istream& is) {
		Binary::Read(is, Mods);
		Binary::Read(is, Score);
		Binary::Read(is, Accuracy);
		Binary::Read(is, MaxCombo);
		Binary::Read(is, BeatmapMaxCombo);
		Binary::Read(is, Rating);
		Binary::Read(is, Mean);
		Binary::Read(is, Error);
		Binary::Read(is, BeatmapHash);
		Binary::Read(is, BeatmapTitle);
		Binary::Read(is, BeatmapVersion);
		Binary::Read(is, PlayerName);
		Binary::Read(is, HealthGraph);
		Binary::Read(is, RatingGraph);
		Binary::Read(is, ResultCounter);
		Binary::Read(is, Events);
	}
};