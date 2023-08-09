#include "OsuBeatmap.h"
#include <algorithm>
#include "DifficultyCalculator.h"
#include "OsuMods.h"


inline double GetTimeDiff(double t) {
	if (t > 0.5)
		return 6 / t;
	if (t < 0)
		return GetTimeDiff(-t);
	return 6 / 0.5;
}
double CalculateDiff(const OsuBeatmap& beatmap, OsuMods mods, int keys) {
	if (beatmap.HitObjects.size() < 10)
		return 0;
	if (keys == 0) {
		keys = (int)beatmap.CircleSize;
	}
	double drain_time = 0;
	std::vector<double> diffs;
	auto objs = beatmap.HitObjects;
	std::sort(objs.begin(), objs.end(), [](auto& a, auto& b) { return a.StartTime < b.StartTime; });
	for (int i = 0; i < objs.size() - 1; i++) {
		auto& ho = objs[i];

		double lastjudge = ho.StartTime;
		double timeDiff = 0;
		if (ho.EndTime != 0) {
			lastjudge = ho.EndTime;
		}
		double nearestSameColumnTime = -1;
		for (int j = i; j < beatmap.HitObjects.size() - 1; j++) {
			auto& ho2 = beatmap.HitObjects[j];
			if (CalcColumn(ho2.X, keys) == CalcColumn(ho.X, keys) && ho2.StartTime > (lastjudge + 1)) {
				nearestSameColumnTime = ho2.StartTime;
				break;
			}
		}
		if (nearestSameColumnTime != -1)
			timeDiff += GetTimeDiff(nearestSameColumnTime - lastjudge);
		auto multi = 1.0;
		double nearestTime = -1;
		for (int j = i; j < beatmap.HitObjects.size() - 1; j++) {
			auto& ho2 = beatmap.HitObjects[j];
			if (ho2.StartTime > (lastjudge + 1)) {
				nearestTime = ho2.StartTime;
				auto between = abs(ho2.StartTime - ho.StartTime);
				if (between < 500)
					drain_time += between;
				break;
			}
		}
		if (nearestTime != -1)
			timeDiff += GetTimeDiff(nearestTime - lastjudge);
		for (int j = 0; j < beatmap.HitObjects.size() - 1; j++) {
			auto& ho2 = beatmap.HitObjects[j];
			if (&ho2 == &ho)
				continue;

			if (std::abs(ho2.StartTime - ho.StartTime) < 0.1 || (ho2.EndTime != 0 && std::abs(ho2.EndTime - ho.StartTime) < 0.1) || (ho2.EndTime != 0 && ho.EndTime != 0 && (std::abs(ho2.EndTime - ho.EndTime) < 0.1 || std::abs(ho2.StartTime - ho.EndTime) < 0.1))) {
				multi *= 1.06;
			}
			if (ho2.EndTime != 0 && ho.StartTime >= ho2.StartTime && ho.StartTime <= ho2.EndTime) {
				multi *= 1.06;
			}
			if (std::abs(nearestTime - ho2.StartTime) < 0.1) {
				auto coldiff = pow(CalcColumn(ho2.X, keys) - CalcColumn(ho.X, keys), 2);
				multi *= 1 + coldiff * 0.04;
			}
		}
		diffs.push_back(timeDiff * multi);
	}
	std::sort(diffs.begin(), diffs.end());
	double avgdiff = 0;
	size_t diffnum = diffs.size();
	double top20diff = 0;
	size_t top20num = diffs.size() / 5;
	for (size_t k = 0; k < diffnum; k++) {
		if (k >= diffnum - top20num) {
			top20diff += diffs[k];
		}
		avgdiff += std::max(0.003, diffs[k]);
	}
	avgdiff /= diffnum; // avgdiff ���������
	avgdiff *= 0.6;
	top20diff /= top20num;
	return (avgdiff + top20diff) * 20;
}