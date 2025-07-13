#pragma once
#include "Monster.h"
#include "PlayerSession.h"
#include "SessionManager.h"
#include "Item.h"
#include "WorldItem.h"

struct GridPos {
	int32 X, Y;

	bool operator==(const GridPos& other) const {
		return X == other.X && Y == other.Y;
	}
};

namespace std {
	template <>
	struct hash<GridPos> {
		size_t operator()(const GridPos& pos) const {
			return std::hash<int32>()(pos.X) ^ (std::hash<int32>()(pos.Y) << 1);
		}
	};
}

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
	void OnMonsterDead(int32 monsterId);
	void UpdateMonsterState(int32 id, ECharacterStateType newState);

	//Tino
	void HandleEarthQuakeImpact(const FVector& rockPos);

	// Inventory
	void PickUpWorldItem(int32 playerId, uint32 itemId);
	void UseInventoryItem(int32 playerId, uint32 itemId);
	void EquipInventoryItem(int32 playerId, uint32 itemId);
	void UnequipInventoryItem(int32 playerId, uint32 itemId);

	// World Item
	bool RemoveWorldItem(uint32 itemId, ZoneType zone);
	std::shared_ptr<WorldItem> FindWorldItemById(uint32 itemId, ZoneType zone);
	void SpawnGoldItem(FVector position, ZoneType zone);

	void SpawnWorldItem(FVector position, uint32 monlv, Type::EPlayer playertype, ZoneType zone);
	void SpawnWorldItem(WorldItem newItem, ZoneType zone);
	void DespawnWorldItem(uint32 itemId, ZoneType zone);

	// Etc
	FVector TransferToZone(int32 playerId, ZoneType targetZone);
	void RespawnPlayer(int32 playerId, ZoneType targetZone);
	void UpdateViewList(std::shared_ptr<Player> player);
	void ClearViewList(std::shared_ptr<Player> player);
	void InitViewList(std::shared_ptr<Player> player, ZoneType zone);
	void AddAllToViewList(std::shared_ptr<Player> player, ZoneType zone);
	bool IsCollisionDetected(ZoneType zone, const FVector& pos, float dist);

	//Quest
	void RequestQuest(int32 playerId, QuestType quest);
	void CompleteQuest(int32 playerId, QuestType quest);
	void QuestSpawn(QuestType quest);
	void BuyItem(int32 playerId, uint8 itemType, uint16 quantity);
	void SellItem(int32 playerId, uint32 itemId);

	void BroadcastToZone(ZoneType zone, Packet* packet);

	void EnterGrid(int32 id, const FVector& pos);
	void LeaveGrid(int32 id, const FVector& pos);
	void MoveGrid(int32 id, const FVector& oldPos, const FVector& newPos);

	std::vector<int32> QueryNearbyCharacters(const FVector& pos);
	GridPos GetGridPos(const FVector& pos);

	std::shared_ptr<Player> GetPlayerByID(int32 id);
	std::shared_ptr<Monster> GetMonsterByID(int32 id);
	std::shared_ptr<Character> GetCharacterByID(int32 id);

	bool IsMonster(int32 id);
	int32 GenerateMonsterId() { return _nextMonsterId++; }
private:
	std::array<std::shared_ptr<Player>, MAX_PLAYER> _players;
	std::unordered_map<ZoneType, std::unordered_map<int32, std::shared_ptr<Player>>> _playersByZone;
	std::unordered_map<ZoneType, std::unordered_map<int32, std::shared_ptr<Monster>>> _monstersByZone;
	std::unordered_map<ZoneType, std::vector<std::shared_ptr<WorldItem>>> _worldItemsByZone;

	std::mutex _mtPlayers;
	std::mutex _mtPlayerZMap;
	std::mutex _mtMonZMap;
	std::mutex _mtItemZMap;
	int32 _nextMonsterId = MAX_PLAYER;
	std::unordered_map<ZoneType, int32> _monsterCnt;

private:

	std::unordered_map<GridPos, std::unordered_set<int32>> _gridMap;
	std::mutex _gridMutex;
};

