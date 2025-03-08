#pragma once

struct ItemStats
{
	int damage;        // 공격력
	int hp;            // 체력
	float critRate;    // 치명타 확률
	float dodgeRate;   // 회피율
	float moveSpeed;   // 이동 속도

	ItemStats(int dmg = 0, int h = 0, float crit = 0.0f, float dodge = 0.0f, float speed = 0.0f)
		: damage(dmg), hp(h), critRate(crit), dodgeRate(dodge), moveSpeed(speed) {
	}
};

struct ItemMeta
{
	int grade;         // 아이템 등급
	bool fromMonster;  // 몬스터 드롭 여부
	int resellPrice;   // 되팔기 가격
	bool isSellable;   // 판매 가능 여부
	bool isBuyable;    // 상점에서 구매 가능 여부

	ItemMeta(int grd = 0, bool fromMob = false, int resell = 0, bool sellable = false, bool buyable = false)
		: grade(grd), fromMonster(fromMob), resellPrice(resell), isSellable(sellable), isBuyable(buyable) {
	}
};

class Item
{
public:
	Item() {};
	Item(uint32 itemId, uint8 itemType) : _itemId(itemId), _itemType(itemType) {}
	uint8 GetItemType() const { return _itemType; }
	uint8 GetItemID() const { return _itemId; }
protected:
	uint32 _itemId;
	uint8 _itemType;
};

class WorldItem;

class InventoryItem : public Item
{
public:
	InventoryItem() {};
	InventoryItem(uint32 itemId, uint8 itemType) : Item(itemId, itemType) {}
	WorldItem ToWorldItem() const;

private:
	ItemStats _stats;
	ItemMeta _meta;
};

class WorldItem : public Item
{
public:
	WorldItem(uint32 itemId, uint8 itemType) : Item(itemId, itemType) {}
	WorldItem(FVector pos);

	uint8 GetRandomItemType();
	Type::EWeapon GetRandomWeapon();
	Type::EArmor GetRandomArmor();
	Type::EUseable GetRandomUseable();
	InventoryItem ToInventoryItem() const;

	void SetPos(FVector pos) { _pos = pos; }
	FVector GetPos() const { return _pos; }
private:
	FVector _pos;
};
