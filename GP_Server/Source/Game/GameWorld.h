#pragma once
#include "Monster.h"
#include "PlayerSession.h"
#include "SessionManager.h"
#include "Item.h"

constexpr size_t MAX_PLAYER = MAX_CLIENT;
constexpr size_t MAX_MONSTER = 10;
constexpr size_t MAX_CHARACTER = MAX_PLAYER + MAX_MONSTER;

class GameWorld
{
public:
	static GameWorld& GetInst()
	{
		static GameWorld inst;
		return inst;
	}

	bool Init();
	void AddPlayer(std::shared_ptr<Character> player);
	void RemoveCharacter(int32 id);

	void CreateMonster();
	void SpawnMonster(PlayerSession& session);

	void PlayerMove(int32 playerId, FVector& pos, uint32 state, uint64& time);
	void PlayerAttack(int32 playerId, float playerYaw);
	void PlayerUseSkill(int32 playerId, ESkillGroup groupId);
	void PlayerSelectCharacter(int32 playerId, Type::EPlayer type);

	void UpdateMonster();
	void BroadcastMonsterStates();

	bool RemoveWorldItem(std::shared_ptr<WorldItem> item);
	std::shared_ptr<WorldItem> FindWorldItemById(uint32 itemId);
	void SpawnWorldItem(FVector position);
	void SpawnWorldItem(WorldItem newItem);

	void PickUpWorldItem(int32 playerId, uint32 itemId);
	void DropInventoryItem(int32 playerId, uint32 itemId);
	void UseInventoryItem(int32 playerId, uint32 itemId);
	void EquipInventoryItem(int32 playerId, uint32 itemId);
	void UnequipInventoryItem(int32 playerId, uint32 itemId);

	void UpdateViewList(std::shared_ptr<Character> character);
	bool IsCollisionDetected(const FVector& pos);
	bool IsCollisionDetected(const FInfoData& target);

	std::shared_ptr<Character> GetCharacterByID(int32 id)
	{
		if (id < 0 || id >= MAX_CHARACTER || !_characters[id])
		{
			LOG(Warning, "Invalid");
			return nullptr;
		}
		return _characters[id];
	}

	FInfoData& GetInfo(int32 id) { return GetCharacterByID(id).get()->GetInfo(); }
	bool IsMonster(int32 id) { return id >= MAX_PLAYER && id < MAX_CHARACTER; }
private:
	std::array<std::shared_ptr<Character>, MAX_CHARACTER> _characters;
	std::vector<std::shared_ptr<WorldItem>> _worldItems;
	std::mutex _carrMutex;
	std::mutex _iMutex;
};

