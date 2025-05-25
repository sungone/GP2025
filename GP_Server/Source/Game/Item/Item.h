#pragma once
#include "ItemTable.h"

struct ItemStats
{
	int damage;
	int hp;
	float critRate;
	float dodgeRate;
	float moveSpeed;

	ItemStats(int dmg = 0, int h = 0, float crit = 0.0f, float dodge = 0.0f, float speed = 0.0f)
		: damage(dmg), hp(h), critRate(crit), dodgeRate(dodge), moveSpeed(speed) {
	}
};

class Item
{
public:
	Item(): _itemID(0),_itemTypeID(0) {};
	Item(uint32 itemID, uint8 itemTypeID);
	uint8 GetItemTypeID() const { return _itemTypeID; }
	uint32 GetItemID() const { return _itemID; }
	const ItemStats& GetStats() const { return _stats; }
	EAbilityType GetAbilityType() const { return _abilityType; }
	float GetAbilityValue() const { return _abilityValue; }
protected:
	uint32 _itemID;
	uint8 _itemTypeID;
	ItemStats _stats;
	EAbilityType _abilityType;
	float _abilityValue;
};

class WorldItem : public Item
{
public:
	WorldItem(FVector pos);
	WorldItem(uint8 itemTypeID);
	WorldItem(uint32 itemId, uint8 itemTypeID) : Item(itemId, itemTypeID) {}
	WorldItem(FVector pos, uint32 monlv, Type::EPlayer playertype);
	uint32 GenerateItemID()
	{
		static int i = 300;
		return i++;
	}
	uint8 GetRandomItemType(uint32 monlv, Type::EPlayer playertype);
	Type::EWeapon GetRandomWeapon(Type::EPlayer playertype);
	Type::EArmor GetRandomArmor();
	Type::EUseable GetRandomBuffTem();
	Type::EUseable GetRandomGold();

	void SetPos(FVector pos) { _pos = pos; }
	FVector GetPos() const { return _pos; }
private:
	FVector _pos;
};
