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
	_info.Stats.Level = 1;
	_info.CollisionRadius = 50.f;
	_info.AttackRadius = 100;
	_info.Speed = 200.f;
	_info.State = ECharacterStateType::STATE_IDLE;
}

void Player::UpdateViewList(std::shared_ptr<Character> other)
{
	if (!other)
	{
		LOG(Warning, "Invalid character!");
		return;
	}

	if (IsInViewDistance(other->GetInfo().Pos, VIEW_DIST))
	{
		if (other->IsMonster())
		{
			AddMonsterToViewList(other);
		}
		else
		{
			AddPlayerToViewList(other);
		}
	}
	else
	{
		if (other->IsMonster())
		{
			RemoveMonsterFromViewList(other);
		}
		else
		{
			RemovePlayerFromViewList(other);
		}
	}
}

void Player::AddMonsterToViewList(std::shared_ptr<Character> monster)
{
	auto monsterId = monster->GetInfo().ID;
	if (!AddToViewList(monsterId)) return;
	auto addPkt = InfoPacket(EPacketType::S_ADD_MONSTER, monster->GetInfo());
	SessionManager::GetInst().SendPacket(_id, &addPkt);
	monster->AddToViewList(_id);
}

void Player::RemoveMonsterFromViewList(std::shared_ptr<Character> monster)
{
	auto monsterId = monster->GetInfo().ID;
	if (!RemoveFromViewList(monsterId)) return;
	auto removePkt = IDPacket(EPacketType::S_REMOVE_MONSTER, monsterId);
	SessionManager::GetInst().SendPacket(_id, &removePkt);
	monster->RemoveFromViewList(_id);
}

void Player::AddPlayerToViewList(std::shared_ptr<Character> otherPlayer)
{
	auto otherPlayerId = otherPlayer->GetInfo().ID;
	if (!AddToViewList(otherPlayerId)) return;
	auto addPkt = InfoPacket(EPacketType::S_ADD_PLAYER, otherPlayer->GetInfo());
	SessionManager::GetInst().SendPacket(_id, &addPkt);

	if (otherPlayer->AddToViewList(_id))
	{
		auto addPkt = InfoPacket(EPacketType::S_ADD_PLAYER, _info);
		SessionManager::GetInst().SendPacket(otherPlayer->GetInfo().ID, &addPkt);
	}
}

void Player::RemovePlayerFromViewList(std::shared_ptr<Character> player)
{
	auto otherPlayerId = player->GetInfo().ID;
	if (!RemoveFromViewList(otherPlayerId)) return;
	auto removePkt = IDPacket(EPacketType::S_REMOVE_PLAYER, otherPlayerId);
	SessionManager::GetInst().SendPacket(_id, &removePkt);

	if (player->RemoveFromViewList(_id))
	{
		auto removePkt = IDPacket(EPacketType::S_REMOVE_PLAYER, _id);
		SessionManager::GetInst().SendPacket(player->GetInfo().ID, &removePkt);
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
	auto dropPos = _info.Pos + _info.GetFrontVector() * 100;
	dropedItem.SetPos(dropPos);
	return dropedItem;
}

void Player::UseItem(uint32 itemId)
{
	auto targetItem = _inventory.FindInventoryItemById(itemId);
	//Todo: 아이템아이디로 플레이어 인벤토리에서 찾아
	// 타입(EUseable type)에 따른 값 스텟에 적용 -> ex)체력키트, 딸바...
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