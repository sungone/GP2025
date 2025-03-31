#include "pch.h"
#include "Player.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Player::Init()
{
	Character::Init();
	_characterClass = ECharacterClass::Player;

	FVector newPos{};
	do { newPos = MapZone::GetInst().GetRandomPos(ZoneType::PLAYGROUND); } while (GameWorld::GetInst().IsCollisionDetected(newPos));
	_info.SetLocation(newPos);
	_info.Stats.Level = 1;
	_info.Stats.Speed = 200.f;
	_info.CollisionRadius = 50.f;

	_info.CharacterType = static_cast<uint8>(Type::EPlayer::GUNNER);
	_info.AttackRadius = (_info.CharacterType == static_cast<uint8>(Type::EPlayer::WARRIOR))?100: 1200;
	_info.State = ECharacterStateType::STATE_IDLE;
	ApplyLevelStats(_info.Stats.Level);
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
	if (!targetItem) return;

	const ItemStats& stats = targetItem->GetStats();
	const ItemMeta& meta = targetItem->GetMeta();
	const EAbilityType ability = targetItem->GetAbilityType();
	const float value = targetItem->GetAbilityValue();

	switch (ability)
	{
	case EAbilityType::Recove:
		_stats.Hp = std::min(_stats.Hp + value, _stats.MaxHp);
		break;

	case EAbilityType::AtcBuff:
		_stats.Damage += value;
		break;

	case EAbilityType::Gold:
		AddGold(value);
		break;

	default:
		break;
	}

	_inventory.RemoveInventoryItemById(itemId);

}

uint8 Player::EquipItem(uint32 itemId)
{
	auto targetItem = _inventory.FindInventoryItemById(itemId);
	if (!targetItem) return 0;

	const ItemStats& itemStats = targetItem->GetStats();
	AddItemStats(itemStats);

	return targetItem->GetItemType();
}

uint8 Player::UnequipItem(uint32 itemId)
{
	auto targetItem = _inventory.FindInventoryItemById(itemId);
	if (!targetItem) return 0;

	const ItemStats& itemStats = targetItem->GetStats();
	RemoveItemStats(itemStats);

	return targetItem->GetItemType();
}

void Player::AddItemStats(const ItemStats& stats)
{
	_stats.Damage += stats.damage;
	_stats.Hp += stats.hp;
	_stats.MaxHp += stats.hp;

	_stats.CrtRate = std::clamp(_stats.CrtRate + stats.critRate, 0.0f, 1.0f);
	_stats.Dodge = std::clamp(_stats.Dodge + stats.dodgeRate, 0.0f, 1.0f);
	_stats.Speed = std::max(0.0f, _stats.Speed + stats.moveSpeed);
}

void Player::RemoveItemStats(const ItemStats& stats)
{
	_stats.Damage = std::max(0.0f, _stats.Damage - stats.damage);
	_stats.Hp = std::max(0.0f, _stats.Hp - stats.hp);
	_stats.MaxHp = std::max(1.0f, _stats.MaxHp - stats.hp);

	_stats.CrtRate = std::clamp(_stats.CrtRate - stats.critRate, 0.0f, 1.0f);
	_stats.Dodge = std::clamp(_stats.Dodge - stats.dodgeRate, 0.0f, 1.0f);
	_stats.Speed = std::max(0.0f, _stats.Speed - stats.moveSpeed);
}

void Player::AddExp(float amount)
{
	_stats.Exp += amount;

	while (_stats.Exp >= _stats.MaxExp)
	{
		_stats.Exp -= _stats.MaxExp;
		LevelUp();
	}
}

void Player::LevelUp()
{
	_stats.Level++;

	ApplyLevelStats(_stats.Level);
}

void Player::ApplyLevelStats(uint32 level)
{
	const FStatData* newStats = PlayerLevelTable::GetInst().GetStatByLevel(level);
	if (!newStats)
	{
		LOG(Warning, "Invaild");
		return;
	}

	_stats.MaxHp = newStats->MaxHp;
	_stats.Hp = _stats.MaxHp;
	_stats.Damage = newStats->Damage;
	_stats.CrtRate = newStats->CrtRate;
	_stats.CrtValue = newStats->CrtValue;
	_stats.Dodge = newStats->Dodge;
	_stats.MaxExp = newStats->MaxExp;
}
