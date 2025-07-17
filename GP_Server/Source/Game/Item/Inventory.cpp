#include "pch.h"
#include "Inventory.h"

bool Inventory::LoadItem(const std::shared_ptr<Item>& item)
{
	return AddItem(item, true);
}

bool Inventory::AddItem(const std::shared_ptr<Item>& item, bool dbSaved)
{
	if (!item) return false;

	uint32 itemId = item->GetItemID();

	if (_items.contains(itemId))
	{
		LOG_W("AddItem failed - duplicate itemID: {}", itemId);
		return false;
	}

	auto inventoryItem = std::make_shared<InventoryItem>();
	inventoryItem->item = std::make_shared<Item>(*item);
	inventoryItem->saved = dbSaved;

	_items[itemId] = inventoryItem;
	if(inventoryItem->item->GetItemTypeID() == Type::EQuestItem::KEY)
		_bHasKey = true;

	return true;
}

bool Inventory::RemoveItem(uint32 itemId)
{
	auto it = _items.find(itemId);
	if (it == _items.end())
	{
		LOG_W("RemoveItem failed - itemId {} not found", itemId);
		return false;
	}

	_items.erase(it);
	return true;
}

std::shared_ptr<Item> Inventory::FindItem(uint32 itemId)
{
	auto it = _items.find(itemId);
	return (it != _items.end()) ? it->second->item : nullptr;
}

bool Inventory::SaveToDB(uint32 dbId)
{
	for (auto& [id, invItem] : _items)
	{
		if (invItem->saved) continue;
		if (DBManager::GetInst().AddUserItem(dbId, invItem->item->GetItemID(), invItem->item->GetItemTypeID()))
		{
			invItem->saved = true;
		}
		else
		{
			return false;
		}
	}
	return true;
}
