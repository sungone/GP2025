#include "pch.h"
#include "Inventory.h"

bool Inventory::AddInventoryItem(const InventoryItem& item)
{
    int itemType = item.GetItemTypeID();
    _slots[itemType].slotItems.emplace_back(std::make_shared<InventoryItem>(item));
    return true;
}

bool Inventory::RemoveInventoryItemById(uint32 itemId)
{
    for (auto& [itemType, slot] : _slots)
    {
        auto& items = slot.slotItems;
        for (auto it = items.begin(); it != items.end(); ++it)
        {
            if ((*it)->GetItemID() == itemId)
            {
                items.erase(it);
                return true;
            }
        }
    }
    return false;
}

std::shared_ptr<InventoryItem> Inventory::FindInventoryItemById(uint32_t itemId)
{
    for (auto& [itemType, slot] : _slots)
    {
        for (auto& item : slot.slotItems)
        {
            if (item->GetItemID() == itemId)
            {
                return item;
            }
        }
    }
    return nullptr;
}