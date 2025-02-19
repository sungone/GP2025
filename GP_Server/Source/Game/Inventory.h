#pragma once
#include "Item.h"

struct ItemSlot
{
    Item item;
    uint32 quantity;
};

class Inventory 
{
public:
    explicit Inventory(int maxSlots) : maxSlots(maxSlots)
    {
        slots.resize(maxSlots);
    }

    bool AddInventoryItem(const Item& item) {};
    bool RemoveInventoryItem(uint32 itemId, uint32 quantity) {};

private:
    int maxSlots;
    std::vector<ItemSlot> slots;
};
