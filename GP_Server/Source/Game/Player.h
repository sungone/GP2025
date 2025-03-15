#pragma once
#include "Character.h"
#include "Inventory.h"

class Player : public Character
{
public:
	virtual void Init() override;

	bool TakeWorldItem(const std::shared_ptr<WorldItem> item);
	WorldItem DropItem(uint32 itemId);

	void UseItem(uint32 itemId);
	uint8 EquipItem(uint32 itemId);
	uint8 UnequipItem(uint32 itemId);
private:
	Inventory _inventory;
	std::unordered_set<int32> _viewList;
};
