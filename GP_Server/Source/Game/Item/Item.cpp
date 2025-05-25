#include "pch.h"
#include "Item.h"

Item::Item(uint32 itemID, uint8 itemTypeID)
	: _itemID(itemID), _itemTypeID(itemTypeID)
{
	auto data = ItemTable::GetInst().GetItemByTypeId(itemTypeID);
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

WorldItem::WorldItem(uint8 itemTypeID)
{
	//Todo: ShopItem�������� ����� -> ���� shopitem��������
	_itemID = GenerateItemID();
	_pos = FVector::ZeroVector;
	_itemTypeID = itemTypeID;
}
WorldItem::WorldItem(FVector pos)
	: _pos(pos)
{
	_itemTypeID = static_cast<uint8>(GetRandomGold());
	_itemID = GenerateItemID();
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
