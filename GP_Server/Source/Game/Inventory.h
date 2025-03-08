#pragma once
#include "Item.h"

struct ItemSlot
{
    std::vector<InventoryItem> items;
};

class Inventory
{
public:
    bool AddInventoryItem(const InventoryItem& item)
    {
        int itemType = item.GetItemType();
        _slots[itemType].items.emplace_back(std::move(item));
        return true;
    }

    bool RemoveInventoryItem(uint32 itemId)
    {
        for (auto& [itemType, slot] : _slots)
        {
            auto& items = slot.items;
            for (auto it = items.begin(); it != items.end(); ++it)
            {
                if (it->GetItemID() == itemId)
                {
                    items.erase(it);
                    return true;
                }
            }
        }
        return false;
    }

private:
    std::unordered_map<int, ItemSlot> _slots;
};
