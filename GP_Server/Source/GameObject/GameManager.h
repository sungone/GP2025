#pragma once
#include "Define.h"
#include "Monster.h"
#include "Session.h"
#include "Timer.h"
#include "SessionManager.h"

constexpr int MAX_CHARACTER = MAX_PLAYER + MAX_MONSTER;

class GameManager
{
public:
	static GameManager& GetInst()
	{
		static GameManager inst;
		return inst;
	}
	void AddPlayer(int id, std::shared_ptr<Character> player);
	void RemoveCharacter(int id);

	void CreateMonster();
	void SpawnMonster(Session& session)
	{
		for (int i = 0;i<MAX_MONSTER;++i)
		{
			monsters[i].Init();
			monsters[i].GetInfo().ID = i;
			auto Pkt = InfoPacket(EPacketType::S_ADD_MONSTER, monsters[i].GetInfo());
			session.DoSend(&Pkt);
		}
		for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
		{
			if (characters[i] && characters[i]->IsValid())
			{
				auto Pkt = InfoPacket(EPacketType::S_ADD_MONSTER, characters[i]->GetInfo());
				session.DoSend(&Pkt);
			}
		}
	}

	void OnDamaged(float damage, FInfoData& damaged)
	{
		auto& monster = GetMonsterInfo(damaged.ID);
		monsters[damaged.ID].OnDamaged(damage);
	}

	FInfoData& GetMonsterInfo(int id) { return monsters[id].GetInfo(); }
private:
	std::array<Monster, MAX_MONSTER> monsters;
	std::array<std::shared_ptr<Character>, MAX_CHARACTER> characters;

#pragma region AIÀÛ¾÷
public :
	Timer MonsterStateBroadcastTimer;
	Timer MonsterAIUpdateTimer;
	std::mutex monsterMutex;

	void StartMonsterStateBroadcast()
	{
		MonsterStateBroadcastTimer.Start(3000, [this]() {
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
		MonsterAIUpdateTimer.Start(4000, [this]() {
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
#pragma endregion

};

