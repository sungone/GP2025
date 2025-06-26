#pragma once
#include "Monster.h"
#include "PlayerSession.h"
#include "SessionManager.h"
#include "Item.h"
#include "WorldItem.h"

class GameWorld
{
public:
	static GameWorld& GetInst()
	{
		static GameWorld inst;
		return inst;
	}

	bool Init();
	// Player
	void PlayerEnterGame(std::shared_ptr<Player> player);
	void PlayerLeaveGame(int32 playerId);
	void PlayerSetLocation(int32 playerId, float yaw, FVector pos);
	void PlayerAddState(int32 playerId, ECharacterStateType newState);
	void PlayerRemoveState(int32 playerId, ECharacterStateType oldState);
	void PlayerMove(int32 playerId, FVector& pos, uint32 state, uint64& time);
	void PlayerAttack(int32 playerId);
	void PlayerUseSkill(int32 playerId, ESkillGroup groupId);
	void PlayerDead(int32 playerID);

	// Monster
	void CreateMonster();
	void RemoveMonster(int32 id);
	void UpdateAllMonsters();

	//Tino
	void HandleEarthQuakeImpact(const FVector& rockPos);

	// Inventory
	void PickUpWorldItem(int32 playerId, uint32 itemId);
	void UseInventoryItem(int32 playerId, uint32 itemId);
	void EquipInventoryItem(int32 playerId, uint32 itemId);
	void UnequipInventoryItem(int32 playerId, uint32 itemId);

	// World Item
	bool RemoveWorldItem(std::shared_ptr<WorldItem> item);
	std::shared_ptr<WorldItem> FindWorldItemById(uint32 itemId);
	void SpawnGoldItem(FVector position);

	void SpawnWorldItem(FVector position, uint32 monlv, Type::EPlayer playertype);
	void SpawnWorldItem(WorldItem newItem);

	// Etc
	FVector TransferToZone(int32 playerId, ZoneType targetZone);
	void RespawnPlayer(int32 playerId, ZoneType targetZone);
	void UpdateViewList(std::shared_ptr<Character> character);
	bool IsCollisionDetected(ZoneType zone, const FVector& pos, float dist);

	//Quest
	void RequestQuest(int32 playerId, QuestType quest);
	void CompleteQuest(int32 playerId, QuestType quest);
	void QuestSpawn(QuestType quest);
	void BuyItem(int32 playerId, uint8 itemType, uint16 quantity);
	void SellItem(int32 playerId, uint32 itemId);


	std::shared_ptr<Player> GetPlayerByID(int32 id);
	std::shared_ptr<Monster> GetMonsterByID(int32 id);
	std::shared_ptr<Character> GetCharacterByID(int32 id);

	bool IsMonster(int32 id);
	int32 GenerateMonsterId() { return _nextMonsterId++; }
private:
	std::array<std::shared_ptr<Player>, MAX_PLAYER> _players;
	std::unordered_map<ZoneType, std::unordered_map<int32, std::shared_ptr<Player>>> _playersByZone;
	std::unordered_map<ZoneType, std::unordered_map<int32, std::shared_ptr<Monster>>> _monstersByZone;
	std::vector<std::shared_ptr<WorldItem>> _worldItems;

	std::mutex _mtPlayers;
	std::mutex _mtPlayerZMap;
	std::mutex _mtMonZMap;
	std::mutex _mtItem;
	int32 _nextMonsterId = MAX_PLAYER;
	std::unordered_map<ZoneType, int32> _monsterCnt;
};

