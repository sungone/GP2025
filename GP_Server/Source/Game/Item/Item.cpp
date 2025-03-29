#include "pch.h"
#include "Item.h"

WorldItem InventoryItem::ToWorldItem() const
{
	WorldItem worldItem(_itemId, _itemType);
	return worldItem;
}


WorldItem::WorldItem(FVector pos) : _pos(pos)
{
	static int i = 300;
	_itemType = GetRandomItemType();
	_itemId = i++;
}

uint8 WorldItem::GetRandomItemType()
{
	uint8 itemCategory = RandomUtils::GetRandomUint8(0, 2);

	switch (itemCategory)
	{
	case EItemCategory::Weapon:
		return static_cast<uint8>(GetRandomWeapon());
	case EItemCategory::Armor:
		return static_cast<uint8>(GetRandomArmor());
	case EItemCategory::Useable:
		return static_cast<uint8>(GetRandomUseable());
	}
}

Type::EWeapon WorldItem::GetRandomWeapon()
{
	return static_cast<Type::EWeapon>(
		RandomUtils::GetRandomUint8(
			static_cast<uint8>(Type::EWeapon::START),
			static_cast<uint8>(Type::EWeapon::END) - 1
		)
		);
}

Type::EArmor WorldItem::GetRandomArmor()
{
	return static_cast<Type::EArmor>(
		RandomUtils::GetRandomUint8(
			static_cast<uint8>(Type::EArmor::START),
			static_cast<uint8>(Type::EArmor::END) - 1
		)
		);
}

Type::EUseable WorldItem::GetRandomUseable()
{
	return static_cast<Type::EUseable>(
		RandomUtils::GetRandomUint8(
			static_cast<uint8>(Type::EUseable::START),
			static_cast<uint8>(Type::EUseable::END) - 1
		)
		);
}

InventoryItem WorldItem::ToInventoryItem() const
{
	InventoryItem invItem(_itemId, _itemType);
	return invItem;
}