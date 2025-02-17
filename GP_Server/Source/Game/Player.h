#pragma once
#include "Character.h"
#include "Inventory.h"

constexpr int MAX_SLOT = 5;
class Player : public Character
{
public:
    Player(): inventory(MAX_SLOT) {}
    Inventory& GetInventory() { return inventory; }

    bool AddItemToInventory(const Item& item) { return inventory.AddItem(item); }
    bool RemoveItemFromInventory(int itemId, int quantity) { return inventory.RemoveItem(itemId, quantity); }
    Item* GetItemFromInventory(int slotIndex) { return inventory.GetItem(slotIndex); }
private:
    Inventory inventory;
};
