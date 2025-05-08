#include "pch.h"
#include "Item.h"
#include "InventoryItem.h"

WorldItem::WorldItem(uint8 itemTypeID)
{
	//Todo: ShopItem생성으로 사용함 -> 추후 shopitem만들어야함
	_itemID = GenerateItemID();
	_pos = FVector::ZeroVector;
	_itemTypeID = itemTypeID;
}
WorldItem::WorldItem(FVector pos, uint32 monlv, Type::EPlayer playertype)
	: _pos(pos)
{
	_itemTypeID = GetRandomItemType(monlv, playertype);
	_itemID = GenerateItemID();
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
		return static_cast<uint8>(GetRandomBuffTem());
	}
}

Type::EWeapon WorldItem::GetRandomWeapon(Type::EPlayer playertype)
{
	if (playertype == Type::EPlayer::WARRIOR)
	{
		return static_cast<Type::EWeapon>(
			RandomUtils::GetRandomUint8(
				static_cast<uint8>(Type::EWeapon::SWORD_FIRST),
				static_cast<uint8>(Type::EWeapon::SWORD_LAST)
			)
			);
	}
	else
	{
		return static_cast<Type::EWeapon>(
			RandomUtils::GetRandomUint8(
				static_cast<uint8>(Type::EWeapon::GUN_FIRST),
				static_cast<uint8>(Type::EWeapon::GUN_LAST)
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

Type::EUseable WorldItem::GetRandomBuffTem()
{
	return static_cast<Type::EUseable>(
		RandomUtils::GetRandomUint8(
			static_cast<uint8>(Type::EUseable::BUFFTEM_FIRST),
			static_cast<uint8>(Type::EUseable::BUFFTEM_LAST)
		)
		);
}

Type::EUseable WorldItem::GetRandomGold()
{
	return static_cast<Type::EUseable>(
		RandomUtils::GetRandomUint8(
			static_cast<uint8>(Type::EUseable::GOLD_FIRST),
			static_cast<uint8>(Type::EUseable::GOLD_LAST)
		)
		);
}

InventoryItem WorldItem::ToInventoryItem() const
{
	InventoryItem invItem(_itemID, _itemTypeID);
	return invItem;
}