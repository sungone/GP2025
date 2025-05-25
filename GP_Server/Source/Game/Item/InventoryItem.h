#pragma once
#include "Item.h"

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

class InventoryItem : public Item
{
public:
	InventoryItem() {};
	InventoryItem(uint32 itemId, uint8 itemType);
	WorldItem ToWorldItem() const;
	const ItemStats& GetStats() const { return _stats; }
	EAbilityType GetAbilityType() const { return _abilityType; }
	float GetAbilityValue() const { return _abilityValue; }

private:
	ItemStats _stats;
	EAbilityType _abilityType;
	float _abilityValue;
};