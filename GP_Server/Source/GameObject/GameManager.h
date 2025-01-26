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

	void OnAttackMonster(Session& session , FInfoData& damaged)
	{
		// std::lock_guard<std::mutex> lock(monsterMutex);

		auto& monster = GetMonsterInfo(damaged.ID);
		auto& damage = session.info.Damage;
		auto& hp = monster.Hp;
		monsters[damaged.ID].OnDamaged(damage);
	}

	FInfoData& GetMonsterInfo(int id) { return monsters[id].GetInfo(); }
	std::array<Monster, MAX_MONSTER> monsters;

public :
	Timer MonsterStateBroadcastTimer;
	Timer MonsterAIUpdateTimer;
	std::mutex monsterMutex;

	void StartMonsterStateBroadcast()
	{
		MonsterStateBroadcastTimer.Start(5000, [this]() {
			BroadcastMonsterStates();
			});
	}

	void StopMonsterStateBroadcast()
	{
		MonsterStateBroadcastTimer.Stop();
	}

	void BroadcastMonsterStates()
	{
		std::lock_guard<std::mutex> lock(monsterMutex);

		for (auto& monster : monsters)
		{
			if (monster.IsHpZero()) { monster.GetInfo().State = ECharacterStateType::STATE_DIE; }
			FInfoData MonsterInfoData = monster.GetInfo();

			MonsterInfoPacket packet(S_MONSTER_STATUS_UPDATE, MonsterInfoData);

			SessionManager::GetInst().Broadcast(&packet);

			std::cout << "Broadcasted state for monster " << monster.GetInfo().ID
				<< " with state " << static_cast<uint32_t>(monster.GetState()) << "." << std::endl;
		}
	}
	
	void StartMonsterAIUpdate()
	{
		MonsterAIUpdateTimer.Start(6000, [this]() {
			for (auto& monster : monsters)
			{
				monster.UpdateBehavior();
			}
			});
	} 

	void StopMonsterAIUpdate()
	{
		MonsterAIUpdateTimer.Stop();
	}
};

