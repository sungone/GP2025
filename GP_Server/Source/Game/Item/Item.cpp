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
	auto data = ItemTable::GetInst().GetItemByTypeId(itemTypeID);
	if (!data) {
		LOG(Warning, "Invalid ItemType");
		_stats = ItemStats{};
		_abilityType = EAbilityType::None;
		_abilityValue = 0.0f;
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
