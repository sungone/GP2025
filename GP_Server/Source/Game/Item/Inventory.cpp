#include "pch.h"
#include "Inventory.h"

bool Inventory::LoadItem(const std::shared_ptr<Item>& item)
{
	if (!item) return false;

	uint32 itemId = item->GetItemID();

	if (_items.contains(itemId))
	{
		LOG_W("LoadItem failed - duplicate itemID: {}", itemId);
		return false;
	}

	auto inventoryItem = std::make_shared<InventoryItem>();
	inventoryItem->item = item;
	inventoryItem->saved = true;

	_items[itemId] = inventoryItem;
	return true;
}

bool Inventory::AddItem(const std::shared_ptr<Item>& item)
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
	inventoryItem->saved = false;

	_items[itemId] = inventoryItem;
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
