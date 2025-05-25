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
	Item() : _itemID(0), _itemTypeID(0) {};
	Item(uint8 itemTypeID);
	Item(uint32 itemID, uint8 itemTypeID);
	void Init(uint8 itemTypeID);
	uint32 GenerateItemID()
	{
		static int i = 300;
		return i++;
	}
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
