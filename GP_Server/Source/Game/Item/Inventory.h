#pragma once
#include "Item.h"

struct ItemSlot
{
    std::vector<std::shared_ptr<Item>> slotItems;
};

class Inventory
{
public:

    bool AddItem(const Item& item);
    bool RemoveItemById(uint32 itemId);
    std::shared_ptr<Item> FindItemById(uint32 itemId);

private:
    std::unordered_map<int, ItemSlot> _slots;
};
