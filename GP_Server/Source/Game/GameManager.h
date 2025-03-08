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

	bool RemoveWorldItem(std::shared_ptr<WorldItem> item);
	std::shared_ptr<WorldItem> FindWorldItemById(uint32 itemId);
	void SpawnWorldItem(FVector position);
	void SpawnWorldItem(WorldItem newItem);

	void PickUpWorldItem(int32 playerId, uint32 itemId);
	void DropInventoryItem(int32 playerId, uint32 itemId);
	void UseInventoryItem(int32 playerId, uint32 itemId);
	void EquipInventoryItem(int32 playerId, uint32 itemId);
	void UnequipInventoryItem(int32 playerId, uint32 itemId);

public:
	Timer _MonsterStateBroadcastTimer;
	Timer _MonsterAIUpdateTimer;

private:
	std::array<std::shared_ptr<Character>, MAX_CHARACTER> _characters;
	std::vector<std::shared_ptr<WorldItem>> _worldItems;
	std::mutex _carrMutex;
	std::mutex _iMutex;
};

