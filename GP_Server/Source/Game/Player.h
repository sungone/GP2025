#pragma once
#include "Character.h"
#include "Inventory.h"

class Player : public Character
{
public:
	Player() : _inventory(MAX_INVENTORY_SLOT) {};
	bool TakeItem(Item item)
	{
		return _inventory.AddInventoryItem(item);
	}
	bool DropItem(uint32 itemId, uint32 quantity = 1)
	{
		return _inventory.RemoveInventoryItem(itemId, quantity);
	};
	void UseItem(Item item) {};
	void EquipItem(Item item) {};
	void UnequipItem(Item item) {};
private:
	Inventory _inventory;
};
