#include "pch.h"
#include "Player.h"

void Player::Init()
{
	Character::Init();
	SetTestRandomLocation();
}

bool Player::TakeWorldItem(const std::shared_ptr<WorldItem> item)
{
	auto invItem = item->ToInventoryItem();
	return _inventory.AddInventoryItem(invItem);
}

WorldItem Player::DropItem(uint32 itemId)
{
	auto targetItem = _inventory.FindInventoryItemById(itemId);
	WorldItem dropedItem = targetItem->ToWorldItem();
	_inventory.RemoveInventoryItemById(itemId);
	dropedItem.SetPos({ _info.Pos.X + 100,  _info.Pos.Y, _info.Pos.Z });
	return dropedItem;
}

void Player::UseItem(uint32 itemId)
{
	auto targetItem = _inventory.FindInventoryItemById(itemId);
	//Todo: �����۾��̵�� �÷��̾� �κ��丮���� ã��
	// Ÿ��(EUseable type)�� ���� �� ���ݿ� ����
}

uint8 Player::EquipItem(uint32 itemId)
{
	auto targetItem = _inventory.FindInventoryItemById(itemId);
	return targetItem->GetItemType();
}

uint8 Player::UnequipItem(uint32 itemId)
{
	auto targetItem = _inventory.FindInventoryItemById(itemId);
	return targetItem->GetItemType();

}