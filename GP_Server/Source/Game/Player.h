#pragma once
#include "Character.h"
#include "Inventory.h"

class Player : public Character
{
public:
    Player(): inventory(MAX_INVENTORY_SLOT) {}
    Inventory& GetInventory() { return inventory; }

    bool AddItemToInventory(const FItem& item) { return inventory.AddItem(item); }
    bool RemoveItemFromInventory(uint32 itemId, uint32 quantity) { return inventory.RemoveItem(itemId, quantity); }
private:
    Inventory inventory;
};
