#pragma once
#include "Item.h"
#include "InventoryItem.h"

struct ItemSlot
{
    std::vector<std::shared_ptr<InventoryItem>> slotItems;
};

class Inventory
{
public:

    bool AddInventoryItem(const InventoryItem& item);
    bool RemoveInventoryItemById(uint32 itemId);
    std::shared_ptr<InventoryItem> FindInventoryItemById(uint32_t itemId);

private:
    std::unordered_map<int, ItemSlot> _slots;
};
