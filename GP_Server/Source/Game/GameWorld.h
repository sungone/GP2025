#pragma once
#include "Monster.h"
#include "PlayerSession.h"
#include "SessionManager.h"
#include "Item.h"

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
	void RemoveCharacter(int32 id);
	void PlayerSetYaw(int32 playerId, float yaw);
	void PlayerAddState(int32 playerId, ECharacterStateType newState);
	void PlayerRemoveState(int32 playerId, ECharacterStateType oldState);
	void PlayerMove(int32 playerId, FVector& pos, uint32 state, uint64& time);
	void PlayerAttack(int32 playerId);
	void PlayerUseSkill(int32 playerId, ESkillGroup groupId);

	// Monster
	void CreateMonster();
	void UpdateMonster();
	void BroadcastMonsterStates();

	// Inventory
	void PickUpWorldItem(int32 playerId, uint32 itemId);
	void DropInventoryItem(int32 playerId, uint32 itemId);
	void UseInventoryItem(int32 playerId, uint32 itemId);
	void EquipInventoryItem(int32 playerId, uint32 itemId);
	void UnequipInventoryItem(int32 playerId, uint32 itemId);

	// World Item
	bool RemoveWorldItem(std::shared_ptr<WorldItem> item);
	std::shared_ptr<WorldItem> FindWorldItemById(uint32 itemId);
	void SpawnWorldItem(FVector position);
	void SpawnWorldItem(WorldItem newItem);

	// Etc
	FVector TransferToZone(int32 playerId, ZoneType targetZone);
	FVector RespawnPlayer(int32 playerId, ZoneType targetZone);
	void UpdateViewList(std::shared_ptr<Character> character);
	bool IsCollisionDetected(const FVector& pos);
	bool IsCollisionDetected(const FInfoData& target);

	std::shared_ptr<Player> GetPlayerByID(int32 id);
	std::shared_ptr<Monster> GetMonsterByID(int32 id);
	std::shared_ptr<Character> GetCharacterByID(int32 id);

	FInfoData& GetInfo(int32 id);
	bool IsMonster(int32 id);
private:
	std::array<std::shared_ptr<Player>, MAX_PLAYER> _players;
	std::unordered_map<int32, std::shared_ptr<Monster>> _monsters;
	std::vector<std::shared_ptr<WorldItem>> _worldItems;

	std::mutex _playerMutex;
	std::mutex _monsterMutex;
	std::mutex _iMutex;

	std::atomic<bool> _updating{ false };
};

