#pragma once

//Todo: 일단 몬스터 죽으면 아이템 스폰되게 해보자
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
	EItem _itemType;
};

//world에 스폰될 아이템(다른 플레이어도 보이는)과 인벤토리 아이템 구분함
// -> world스폰 아이템은 id 통해 구분
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