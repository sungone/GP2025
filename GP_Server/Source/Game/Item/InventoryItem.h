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

struct ItemMeta
{
	int grade;
	bool fromMonster;
	int resellPrice;
	bool isSellable;
	bool isBuyable;

	ItemMeta(int grd = 0, bool fromMob = false, int resell = 0, bool sellable = false, bool buyable = false)
		: grade(grd), fromMonster(fromMob), resellPrice(resell), isSellable(sellable), isBuyable(buyable) {
	}
};

class InventoryItem : public Item
{
public:
	InventoryItem() {};
	InventoryItem(uint32 itemId, uint8 itemType) : Item(itemId, itemType)
	{
		auto data = ItemTable::GetInst().GetItemByTypeId(itemId);
		_stats = ItemStats(
			static_cast<int>(data->Damage),
			static_cast<int>(data->Hp),
			data->CrtRate,
			data->DodgeRate,
			data->MoveSpeed
		);

		_meta = ItemMeta(
			data->Grade,
			data->FromMonster,
			data->ResellPrice,
			data->bSellable,
			data->bBuyable
		);
	}
	WorldItem ToWorldItem() const;
	const ItemStats& GetStats() const { return _stats; }
	const ItemMeta& GetMeta() const { return _meta; }
private:
	ItemStats _stats;
	ItemMeta _meta;
};