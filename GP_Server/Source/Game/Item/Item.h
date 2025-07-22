#pragma once
#include "ItemTable.h"

struct ItemStats
{
	float damage;
	float hp;
	float critRate;
	float dodgeRate;
	float moveSpeed;

	ItemStats(float dmg = 0, float h = 0, float crit = 0.0f, float dodge = 0.0f, float speed = 0.0f)
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
		//Todo:DB에서 중복 안되게 DB에서 시작 id받아서 처리해야
		static int i = 300;
		return i++;
	}
	uint8 GetItemTypeID() const { return _itemTypeID; }
	uint32 GetItemID() const { return _itemID; }
	const ItemStats& GetStats() const;
	EAbilityType GetAbilityType() const;
	float GetAbilityValue() const;
	EItemCategory GetItemCategory() const;
protected:
	uint32 _itemID;
	uint8 _itemTypeID;
	const FItemData* _itemInfo;
};
