#pragma once
#include "ItemTable.h"

class Item
{
public:
	Item(): _itemId(0),_itemType(0) {};
	Item(uint32 itemId, uint8 itemType) : _itemId(itemId), _itemType(itemType) {}
	uint8 GetItemType() const { return _itemType; }
	uint32 GetItemID() const { return _itemId; }
protected:
	uint32 _itemId;
	uint8 _itemType;
};

class InventoryItem;
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
