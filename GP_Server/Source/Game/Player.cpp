#include "pch.h"
#include "Player.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Player::Init()
{
	Character::Init();
	_characterType = CharacterType::Player;

	FVector newPos{};
	do { newPos = MapZone::GetInst().GetRandomPos(ZoneType::PLAYGROUND); } while (GameWorld::GetInst().IsCollisionDetected(newPos));
	_info.SetLocation(newPos);
}

void Player::UpdateViewList(std::shared_ptr<Character> other)
{
	if (!other) { LOG(Warning, "Invaild!"); return; }

	auto otherId = other->GetInfo().ID;

	if (IsInViewDistance(other->GetInfo().Pos, VIEW_DIST))
	{
		bool res = AddToViewList(otherId);
		if (!res) return;
		if (other->IsMonster())
		{
			auto addPkt = InfoPacket(EPacketType::S_ADD_MONSTER, other->GetInfo());
			SessionManager::GetInst().SendPacket(_id, &addPkt);
			other->AddToViewList(_id);
		}
		else
		{
			auto addPkt = InfoPacket(EPacketType::S_ADD_PLAYER, other->GetInfo());
			SessionManager::GetInst().SendPacket(_id, &addPkt);
			if (other->AddToViewList(_id))
			{
				auto addPkt = InfoPacket(EPacketType::S_ADD_PLAYER, _info);
				SessionManager::GetInst().SendPacket(other->GetInfo().ID, &addPkt);
			}
		}

	}
	else
	{
		bool res = RemoveFromViewList(other->GetInfo().ID);
		if (!res) return;
		if (other->IsMonster())
		{
			auto removePkt = IDPacket(EPacketType::S_REMOVE_MONSTER, otherId);
			SessionManager::GetInst().SendPacket(_id, &removePkt);
		}
		else
		{
			auto removePkt = IDPacket(EPacketType::S_REMOVE_PLAYER, otherId);
			SessionManager::GetInst().SendPacket(_id, &removePkt);
			if (other->RemoveFromViewList(_id))
			{
				auto removePkt = IDPacket(EPacketType::S_REMOVE_PLAYER, _id);
				SessionManager::GetInst().SendPacket(other->GetInfo().ID, &removePkt);
			}
		}
	}
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
	//Todo: 아이템아이디로 플레이어 인벤토리에서 찾아
	// 타입(EUseable type)에 따른 값 스텟에 적용
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