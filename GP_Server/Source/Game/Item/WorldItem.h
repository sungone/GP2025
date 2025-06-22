#pragma once
#include "ItemTable.h"
#include "Item.h"
class WorldItem : public Item
{
public:
	WorldItem(uint8 itemTypeID);
	WorldItem(FVector pos);
	WorldItem(uint8 itemTypeID, FVector pos);
	WorldItem(FVector pos, uint32 monlv, Type::EPlayer playertype);

	uint8 GetRandomItemType(uint32 monlv, Type::EPlayer playertype);
	Type::EWeapon GetRandomWeapon(Type::EPlayer playertype);
	Type::EArmor GetRandomArmor();
	Type::EUseable GetRandomBuffTem();
	Type::EUseable GetRandomQuestTem();//for test
	Type::EUseable GetRandomGold();

	void SetPos(FVector pos) { _pos = pos; }
	FVector GetPos() const { return _pos; }
private:
	FVector _pos;
};
