#pragma once
#include "Define.h"
#include "Monster.h"
#include "Session.h"
#include "Timer.h"
#include "SessionManager.h"

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
	std::array<Monster, MAX_MONSTER> monsters;

public :
	Timer timer;
	std::mutex monsterMutex;

	void StartMonsterStateBroadcast()
	{
		timer.Start(5000, [this]() {
			BroadcastMonsterStates();
			});
	}

	void StopMonsterStateBroadcast()
	{
		timer.Stop();
	}

	void BroadcastMonsterStates()
	{
		std::lock_guard<std::mutex> lock(monsterMutex);

		for (auto& monster : monsters)
		{
			FMonsterStateData stateData = { monster.GetInfo().ID, static_cast<uint32_t>(monster.GetState()) };
			MonsterStatePacket packet(S_MONSTER_STATUS_UPDATE, stateData);

			SessionManager::GetInst().Broadcast(&packet);

			// µð¹ö±ë ·Î±×
			std::cout << "Broadcasted state for monster " << monster.GetInfo().ID
				<< " with state " << static_cast<uint32_t>(monster.GetState()) << "." << std::endl;
		}
	}

	void StartMonsterAIUpdate()
	{
		timer.Start(500, [this]() {
			for (auto& monster : monsters)
			{
				monster.UpdateBehavior();
			}
			});
	}
};

