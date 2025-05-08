#pragma once
#include "ItemTable.h"

class Item
{
public:
	Item(): _itemID(0),_itemTypeID(0) {};
	Item(uint32 itemID, uint8 itemTypeID) : _itemID(itemID), _itemTypeID(itemTypeID) {}
	uint8 GetItemTypeID() const { return _itemTypeID; }
	uint32 GetItemID() const { return _itemID; }
protected:
	uint32 _itemID;
	uint8 _itemTypeID;
};

class InventoryItem;
class WorldItem : public Item
{
public:
	WorldItem(uint8 itemTypeID);
	WorldItem(uint32 itemId, uint8 itemTypeID) : Item(itemId, itemTypeID) {}
	WorldItem(FVector pos, uint32 monlv, Type::EPlayer playertype);
	uint32 GenerateItemID()
	{
		static int i = 300;
		return i++;
	}
	uint8 GetRandomItemType(uint32 monlv, Type::EPlayer playertype);
	Type::EWeapon GetRandomWeapon(Type::EPlayer playertype);
	Type::EArmor GetRandomArmor();
	Type::EUseable GetRandomBuffTem();
	Type::EUseable GetRandomGold();
	InventoryItem ToInventoryItem() const;

	void SetPos(FVector pos) { _pos = pos; }
	FVector GetPos() const { return _pos; }
private:
	FVector _pos;
};
