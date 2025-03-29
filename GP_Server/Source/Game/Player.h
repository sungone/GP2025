#pragma once
#include "Character.h"
#include "Inventory.h"

class Player : public Character
{
public:
	void Init() override;
	void UpdateViewList(std::shared_ptr<Character> other) override;
	void AddMonsterToViewList(std::shared_ptr<Character> monster);
	void AddPlayerToViewList(std::shared_ptr<Character> player);
	void RemoveMonsterFromViewList(std::shared_ptr<Character> monster);
	void RemovePlayerFromViewList(std::shared_ptr<Character> player);

	bool TakeWorldItem(const std::shared_ptr<WorldItem> item);
	WorldItem DropItem(uint32 itemId);

	void UseItem(uint32 itemId);
	uint8 EquipItem(uint32 itemId);
	uint8 UnequipItem(uint32 itemId);

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
	Inventory _inventory;
	std::unordered_set<int32> _viewList;
};
