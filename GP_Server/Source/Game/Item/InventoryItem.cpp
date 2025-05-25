#include "pch.h"
#include "InventoryItem.h"

InventoryItem::InventoryItem(uint32 itemId, uint8 itemType)
	: Item(itemId, itemType)
{
	auto data = ItemTable::GetInst().GetItemByTypeId(itemType);
	if (!data)
	{
		LOG(Warning, "Invaild");
		return;
	}

	_stats = ItemStats(
		static_cast<int>(data->Damage),
		static_cast<int>(data->Hp),
		data->CrtRate,
		data->DodgeRate,
		data->MoveSpeed
	);

	_abilityType = data->AbilityType;
	_abilityValue = data->AbilityValue;
}

WorldItem InventoryItem::ToWorldItem() const
{
	WorldItem worldItem(_itemID, _itemTypeID);
	return worldItem;
}
