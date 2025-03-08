#pragma once
#include "Monster.h"
#include "Session.h"
#include "Timer.h"
#include "SessionManager.h"
#include "Item.h"

constexpr size_t MAX_PLAYER = MAX_CLIENT;
constexpr size_t MAX_MONSTER = 50;
constexpr size_t MAX_CHARACTER = MAX_PLAYER + MAX_MONSTER;

class GameManager
{
public:
	static GameManager& GetInst()
	{
		static GameManager inst;
		return inst;
	}

	bool Init();
	void AddPlayer(std::shared_ptr<Character> player);
	void RemoveCharacter(int32 id);

	void CreateMonster();
	void SpawnMonster(Session& session);

	void ProcessAttack(int32 attackerId, int32 attackedId);
	std::shared_ptr<Character> GetCharacterByID(int32 id);
	FInfoData& GetInfo(int32 id) { return GetCharacterByID(id).get()->GetInfo(); }

	void StartMonsterStateBroadcast();
	void BroadcastMonsterStates();
	void UpdateMonster();

	void SpawnItem(FVector position);
	void RemoveItem(uint32_t itemId);

public:
	Timer _MonsterStateBroadcastTimer;
	Timer _MonsterAIUpdateTimer;
	std::mutex _carrMutex;

private:
	std::array<std::shared_ptr<Character>, MAX_CHARACTER> _characters;
	std::vector<std::shared_ptr<WorldItem>> _worldItems;
	std::mutex _iMutex;
};

