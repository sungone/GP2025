#include "pch.h"
#include "InventoryItem.h"

InventoryItem::InventoryItem(uint32 itemId, uint8 itemType)
	: Item(itemId, itemType)
{
	auto data = ItemTable::GetInst().GetItemByTypeId(itemId);
	_stats = ItemStats(
		static_cast<int>(data->Damage),
		static_cast<int>(data->Hp),
		data->CrtRate,
		data->DodgeRate,
		data->MoveSpeed
	);

	_meta = ItemMeta(
		data->Grade,
		data->FromMonster,
		data->ResellPrice,
		data->bSellable,
		data->bBuyable
	);
	_abilityType = data->AbilityType;
	_abilityValue = data->AbilityValue;
}

WorldItem InventoryItem::ToWorldItem() const
{
	WorldItem worldItem(_itemId, _itemType);
	return worldItem;
}
