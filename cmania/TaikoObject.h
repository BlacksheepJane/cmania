#pragma once
#include "AudioManager.h"
#include "HitObject.h"
struct TaikoObject : public HitObject {
	enum {
		Don = 0,
		Kat = 1,// 0λ
		Large = 2,// 1λ
		Spinner = 4,// 2λ
	} ObjectType;
	double EndTime;
	int RemainsHits;
	int TotalHits;
	double TickTime;
	// �� 1 Ϊ��λ(
	double Velocity;
};