#include "pch.h"
#include "Item.h"
#include "InventoryItem.h"
WorldItem::WorldItem(FVector pos, uint32 monlv, Type::EPlayer playertype)
	: _pos(pos)
{
	static int i = 300;
	_itemTypeID = GetRandomItemType(monlv, playertype);
	_itemID = i++;
}

uint8 WorldItem::GetRandomItemType(uint32 monlv, Type::EPlayer playertype)
{
	uint8 itemCategory = RandomUtils::GetRandomUint8(0, 2);

	switch (itemCategory)
	{
	case static_cast<uint8>(EItemCategory::Weapon):
		return static_cast<uint8>(GetRandomWeapon(playertype));
	case static_cast<uint8>(EItemCategory::Armor):
		return static_cast<uint8>(GetRandomArmor());
	case static_cast<uint8>(EItemCategory::Useable):
		return static_cast<uint8>(GetRandomUseable());
	}
}

Type::EWeapon WorldItem::GetRandomWeapon(Type::EPlayer playertype)
{
	if (playertype == Type::EPlayer::WARRIOR)
	{
		return static_cast<Type::EWeapon>(
			RandomUtils::GetRandomUint8(
				static_cast<uint8>(Type::EWeapon::SWORD_FIRST),
				static_cast<uint8>(Type::EWeapon::SWORD_LAST) - 1
			)
			);
	}
	else
	{
		return static_cast<Type::EWeapon>(
			RandomUtils::GetRandomUint8(
				static_cast<uint8>(Type::EWeapon::GUN_FIRST),
				static_cast<uint8>(Type::EWeapon::GUN_LAST) - 1
			)
			);
	}
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
	InventoryItem invItem(_itemID, _itemTypeID);
	return invItem;
}