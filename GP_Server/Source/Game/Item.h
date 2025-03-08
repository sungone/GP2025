#pragma once

struct ItemStats
{
	int damage;        // ���ݷ�
	int hp;            // ü��
	float critRate;    // ġ��Ÿ Ȯ��
	float dodgeRate;   // ȸ����
	float moveSpeed;   // �̵� �ӵ�

	ItemStats(int dmg = 0, int h = 0, float crit = 0.0f, float dodge = 0.0f, float speed = 0.0f)
		: damage(dmg), hp(h), critRate(crit), dodgeRate(dodge), moveSpeed(speed) {
	}
};

struct ItemMeta
{
	int grade;         // ������ ���
	bool fromMonster;  // ���� ��� ����
	int resellPrice;   // ���ȱ� ����
	bool isSellable;   // �Ǹ� ���� ����
	bool isBuyable;    // �������� ���� ���� ����

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
