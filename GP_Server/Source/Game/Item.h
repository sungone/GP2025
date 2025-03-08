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

class InventoryItem : public Item
{
public:
	InventoryItem() {};
	InventoryItem(uint32 itemId, uint8 itemType) : Item(itemId, itemType) {}
private:
	ItemStats _stats;
	ItemMeta _meta;
};

class WorldItem : public Item
{
public:
	WorldItem(FVector pos)
		: _pos(pos)
	{
		static int i = 300;
		_itemType = GetRandomItemType();
		_itemId = i++;
	}

	uint8 GetRandomItemType()
	{
		uint8 itemCategory = RandomUtils::GetRandomUint8(0, 2);

		switch (itemCategory)
		{
		case EItemCategory::Weapon:
			return static_cast<uint8>(GetRandomWeapon());
		case EItemCategory::Armor:
			return static_cast<uint8>(GetRandomArmor());
		case EItemCategory::Useable:
			return static_cast<uint8>(GetRandomUseable());
		}
	}

	Type::EWeapon GetRandomWeapon()
	{
		return static_cast<Type::EWeapon>(
			RandomUtils::GetRandomUint8(
				static_cast<uint8>(Type::EWeapon::START),
				static_cast<uint8>(Type::EWeapon::END) - 1
			)
			);
	}

	Type::EArmor GetRandomArmor()
	{
		return static_cast<Type::EArmor>(
			RandomUtils::GetRandomUint8(
				static_cast<uint8>(Type::EArmor::START),
				static_cast<uint8>(Type::EArmor::END) - 1
			)
			);
	}

	Type::EUseable GetRandomUseable()
	{
		return static_cast<Type::EUseable>(
			RandomUtils::GetRandomUint8(
				static_cast<uint8>(Type::EUseable::START),
				static_cast<uint8>(Type::EUseable::END) - 1
			)
			);
	}

	InventoryItem ToInventoryItem() const
	{
		InventoryItem invItem(_itemId, _itemType);
		return invItem;
	}

private:
	FVector _pos;
};
