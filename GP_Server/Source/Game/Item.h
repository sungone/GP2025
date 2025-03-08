#pragma once

//Todo: �ϴ� ���� ������ ������ �����ǰ� �غ���
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
	uint8 GetItemType() { return _itemType; }
protected:
	uint8 _itemType;
};

//world�� ������ ������(�ٸ� �÷��̾ ���̴�)�� �κ��丮 ������ ������
// -> world���� �������� id ���� ����


enum EItemCategory
{
	Weapon,
	Armor,
	Useable,
	Quest
};

class WorldItem : Item
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

	uint32 GetItemId() { return _itemId; }

private:
	uint32 _itemId;
	FVector _pos;
};

class InventoryItem :Item
{
public:
	ItemStats _stats;
	ItemMeta _meta;
};