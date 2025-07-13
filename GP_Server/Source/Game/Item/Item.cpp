#include "pch.h"
#include "Item.h"

Item::Item(uint8 itemTypeID)
	: _itemID(GenerateItemID())
{
	Init(itemTypeID);
}

Item::Item(uint32 itemID, uint8 itemTypeID)
	: _itemID(itemID)
{
	Init(itemTypeID);
}

void Item::Init(uint8 itemTypeID)
{
	_itemTypeID = itemTypeID;
	_itemInfo = ItemTable::GetInst().GetItemByTypeId(itemTypeID);
	if (!_itemInfo) {
		LOG_W("Invalid ItemType");
		return;
	}
}

const ItemStats& Item::GetStats() const
{
	return ItemStats(
		_itemInfo->Damage,
		_itemInfo->Hp,
		_itemInfo->CrtRate,
		_itemInfo->DodgeRate,
		_itemInfo->MoveSpeed
	);
}

EAbilityType Item::GetAbilityType() const
{
	return _itemInfo->AbilityType;
}

float Item::GetAbilityValue() const
{
	return _itemInfo->AbilityValue;
}

EItemCategory Item::GetItemCategory() const
{
	return _itemInfo->Category;
}
