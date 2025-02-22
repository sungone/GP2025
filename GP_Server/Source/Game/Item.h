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
	EItem _itemType;
};

//world�� ������ ������(�ٸ� �÷��̾ ���̴�)�� �κ��丮 ������ ������
// -> world���� �������� id ���� ����
class WorldItem : Item
{
public:
    WorldItem(EItem itemType, uint32_t itemId, FVector pos)
        : _itemId(itemId), _pos(pos)
    {
        _itemType = itemType;
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