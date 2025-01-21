#pragma once
#include "Define.h"
#include "Monster.h"

class GameObjectManager
{
public:
	static GameObjectManager& GetInst()
	{
		static GameObjectManager inst;
		return inst;
	}
	void SetBunkerRandomLocation(FStatusData& info)
	{
		static std::default_random_engine dre;
		static std::uniform_real_distribution<float> ud_x(-3000, -1000);
		static std::uniform_real_distribution<float> ud_y(-3500, -1500);

		info.X = ud_x(dre);
		info.Y = ud_y(dre);
		info.Z = 116;
	}

private:
	std::array<Monster, MAX_MONSTER> Monsters;
};

