#include "pch.h"
#include "Inventory.h"
#include "ScopedDBSession.h"

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
	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");

		sess.startTransaction();

		uint32 nextItemId = 1;

		auto res = sess.sql("SELECT IFNULL(MAX(item_id), 0) + 1 FROM user_items WHERE user_id = ?")
			.bind(dbId)
			.execute();
		auto row = res.fetchOne();
		if (row) nextItemId = static_cast<uint32>(row[0].get<int>());

		for (auto& [id, invItem] : _items)
		{
			if (invItem->saved) continue;

			schema.getTable("user_items")
				.insert("user_id", "item_id", "item_type_id")
				.values(dbId, nextItemId++, invItem->item->GetItemTypeID())
				.execute();

			invItem->saved = true;
		}

		sess.commit();
		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("Inventory::SaveToDB Error: {}", e.what());
		return false;
	}
}
