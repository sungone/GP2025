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
			auto Pkt = InfoPacket(EPacketType::S_ADD_MONSTER, m.GetInfo());
			session.DoSend(&Pkt);
		}
	}
	void OnAttackMonster(Session& session , FInfoData& atkInfo)
	{
		auto& monster = GetMonsterInfo(atkInfo.ID);
		auto& damage = session.info.Damage;
		auto& hp = monster.Hp;
		LOG(LogType::Log, std::format("before : damage[{}] = {}, hp[{}] = {}",
			session.id,damage,monster.ID, hp));

		if ((hp - damage) <= 0)
			hp = 0;
		else
		{
			hp -= damage;
		}
		LOG(LogType::Log, std::format("after : damage[{}] = {}, hp[{}] = {}",
			session.id, damage, monster.ID, hp));

	}
	FInfoData& GetMonsterInfo(int id) { return monsters[id].GetInfo(); }
private:
	std::array<Monster, MAX_MONSTER> monsters;
};

