#pragma once
#include "Define.h"
#include "Monster.h"
#include "Session.h"

class GameManager
{
public:
	static GameManager& GetInst()
	{
		static GameManager inst;
		return inst;
	}

	void SpawnMonster(Session& session)
	{
		for (auto& m : monsters)
		{
			static int i;
			m.Init();
		}
		for (auto& m : monsters)
		{
			auto Pkt = InfoPacket(EPacketType::S_ADD_MONSTER, m.GetInfo());
			session.DoSend(&Pkt);
		}
	}

private:
	std::array<Monster, MAX_MONSTER> monsters;
};

