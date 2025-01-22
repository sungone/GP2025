#pragma once
#include "Define.h"
#include "Monster.h"

class GameManager
{
public:
	static GameManager& GetInst()
	{
		static GameManager inst;
		return inst;
	}

	void SpawnMonster()
	{
		for (auto& m : monsters)
		{
			m.Init();
		}
	}

private:
	std::array<Monster, MAX_MONSTER> monsters;
};

