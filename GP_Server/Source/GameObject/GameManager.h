#pragma once
#include "Define.h"
#include "Monster.h"
#include "Session.h"
#include "Timer.h"
#include "SessionManager.h"

constexpr size_t MAX_PLAYER = MAX_CLIENT;
constexpr size_t MAX_MONSTER = 10;
constexpr size_t MAX_CHARACTER = MAX_PLAYER + MAX_MONSTER;

class GameManager
{
public:
	static GameManager& GetInst()
	{
		static GameManager inst;
		return inst;
	}
	
	void Init();
	void AddPlayer(int32 id, std::shared_ptr<Character> player);
	void RemoveCharacter(int32 id);

	void CreateMonster();
	void SpawnMonster(Session& session);

	bool OnDamaged(float damage, FInfoData& damaged);

	std::shared_ptr<Character> GetCharacterByID(int32 id);
	FInfoData& GetInfo(int32 id) { return GetCharacterByID(id).get()->GetInfo(); }

private:
	std::array<std::shared_ptr<Character>, MAX_CHARACTER> characters;

#pragma region AIÀÛ¾÷
public:
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

		for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
		{
			if (characters[i] && characters[i]->IsValid())
			{
				auto& monster = characters[i];
				if (monster->IsDead())
				{
					LOG(SendLog, std::format("Remove monster[{}]", monster->GetInfo().ID));
					monster->GetInfo().State = ECharacterStateType::STATE_DIE;
					RemoveCharacter(i);
					return;
				}
				FInfoData MonsterInfoData = monster->GetInfo();
				MonsterInfoPacket packet(S_MONSTER_STATUS_UPDATE, MonsterInfoData);
				SessionManager::GetInst().Broadcast(&packet);
				LOG(SendLog, std::format("Update monster[{}]", monster->GetInfo().ID));
			}
		}
	}

	void StartMonsterAIUpdate()
	{
		MonsterAIUpdateTimer.Start(4000, [this]() {
			for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
			{
				if (characters[i] && characters[i]->IsValid())
				{
					characters[i]->Update();
				}
			}
			});
	}

	void StopMonsterAIUpdate()
	{
		MonsterAIUpdateTimer.Stop();
	}
#pragma endregion

};

