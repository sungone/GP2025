#pragma once
#include "Character.h"
#include "Inventory.h"

class Player : public Character
{
public:
	void Init() override;
	void SaveToDB();
	void SetCharacterType(Type::EPlayer type);

	void UpdateViewList(std::shared_ptr<Character> other) override;
	void AddMonsterToViewList(std::shared_ptr<Character> monster);
	void AddPlayerToViewList(std::shared_ptr<Character> player);
	void RemoveMonsterFromViewList(std::shared_ptr<Character> monster);
	void RemovePlayerFromViewList(std::shared_ptr<Character> player);

	bool TakeWorldItem(const std::shared_ptr<WorldItem> item);
	WorldItem DropItem(uint32 itemId);
	bool Attack(std::shared_ptr<Character> target);

	void UseSkill(ESkillGroup groupId);
	void ExecuteSkillEffect(const FSkillData& skill);
	void LearnSkill(ESkillGroup groupId);
	void UpgradeSkill(ESkillGroup groupId);

	void UseItem(uint32 itemId);
	uint8 EquipItem(uint32 itemId);
	uint8 UnequipItem(uint32 itemId);

	void AddItemStats(const ItemStats& stats);
	void RemoveItemStats(const ItemStats& stats);

	void AddExp(float amount);
	void LevelUp();
	void ApplyLevelStats(uint32 level);

	void AddGold(int amount) { _gold += amount; }
	bool SpendGold(int amount)
	{
		if (_gold < amount) return false;
		_gold -= amount;
		return true;
	}
	int GetGold() const { return _gold; }
	float GetAttackDamage() override
	{
		return _info.GetAttackDamage(RandomUtils::GetRandomFloat(0.0f, 1.0f));
	}
	void ChangeState(ECharacterStateType newState) override
	{
		if (!_info.HasState(newState))
		{
			_info.AddState(newState);
		}
	}
private:
	uint32 _dbId;
	Type::EPlayer _playerType;
	Inventory _inventory;
	std::unordered_set<int32> _viewList;
	FStatData& _stats = _info.Stats;
	uint32& _gold = _info.Gold;
	std::unordered_map<ESkillGroup, uint32> _skillLevels;
};
