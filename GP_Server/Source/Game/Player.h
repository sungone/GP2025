#pragma once
#include "Character.h"
#include "Inventory.h"

class Player : public Character
{
public:
	virtual void Init() override;

	bool TakeItem(const InventoryItem& item)
	{
		return _inventory.AddInventoryItem(item);
	}
	bool DropItem(uint32 itemId)
	{
		return _inventory.RemoveInventoryItem(itemId);
	};
	void UseItem(InventoryItem item) {};
	void EquipItem(InventoryItem item) {};
	void UnequipItem(InventoryItem item) {};
private:
	Inventory _inventory;
};
