#pragma once
#include "Item.h"

struct ItemSlot
{
    std::vector<std::shared_ptr<Item>> slotItems;
};

class Inventory
{
public:

    bool AddInventoryItem(const Item& item);
    bool RemoveInventoryItemById(uint32 itemId);
    std::shared_ptr<Item> FindInventoryItemById(uint32_t itemId);

private:
    std::unordered_map<int, ItemSlot> _slots;
};
