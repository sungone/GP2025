#include "pch.h"
#include "InventoryItem.h"

WorldItem InventoryItem::ToWorldItem() const
{
	WorldItem worldItem(_itemId, _itemType);
	return worldItem;
}
