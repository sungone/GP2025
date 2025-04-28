#include "pch.h"
#include "Player.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Player::Init()
{
	Character::Init();
	_characterClass = ECharacterClass::Player;
#ifndef DB_LOCAL
	_info.SetName(L"플레이어");
	SetCharacterType(Type::EPlayer::WARRIOR);
	_info.Stats.Level = 1;
	_info.Stats.Speed = 200.f;
	_info.CollisionRadius = 50.f;
	_info.State = ECharacterStateType::STATE_IDLE;
	ApplyLevelStats(_info.Stats.Level);
	FVector newPos{};
	do { newPos = MapZone::GetInst().GetRandomPos(ZoneType::PLAYGROUND); } while (GameWorld::GetInst().IsCollisionDetected(_info));
	_info.SetLocation(newPos);
#endif
}

void Player::SaveToDB(uint32 dbId)
{
#ifdef DB_LOCAL
	DBManager::GetInst().UpdatePlayerInfo(dbId, _info);
#endif
}

void Player::SetCharacterType(Type::EPlayer type)
{
	LOG(std::format("Set type {}", (type == Type::EPlayer::WARRIOR) ? "warrior" : "gunner"));
	_playerType = type;
	_info.CharacterType = static_cast<uint8>(_playerType);
	if (_playerType == Type::EPlayer::WARRIOR)
	{
		_info.fovAngle = 90;
		_info.AttackRadius = 300;
		/*LearnSkill(ESkillGroup::HitHard);
		LearnSkill(ESkillGroup::Clash);
		LearnSkill(ESkillGroup::Whirlwind);*/
	}
	else
	{
		_info.fovAngle = 10;
		_info.AttackRadius = 1500;
		/*LearnSkill(ESkillGroup::Throwing);
		LearnSkill(ESkillGroup::FThrowing);
		LearnSkill(ESkillGroup::Anger);*/
	}
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
	float detectDist = 500.f;
	if (!IsCollision(item->GetPos(), detectDist))
		return false;
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

bool Player::Attack(std::shared_ptr<Character> monster)
{
	if (!IsInAttackRange(monster->GetInfo()))
		return false;

	//float atkDamage = GetAttackDamage();
	//for test
	float atkDamage = 100;
	if (atkDamage > 0.0f)
	{
		monster->OnDamaged(atkDamage);
	}

	auto pkt = MonsterDamagePacket(monster->GetInfo(), atkDamage);
	SessionManager::GetInst().SendPacket(_id, &pkt);
	SessionManager::GetInst().BroadcastToViewList(&pkt, _id);
	return true;
}

void Player::UseSkill(ESkillGroup groupId)
{
	if (groupId == ESkillGroup::None)
	{
		LOG("GetSkillGropID is None");
		return;
	}
	auto it = _skillLevels.find(groupId);
	if (it == _skillLevels.end())
	{
		LOG(Warning, std::format("Player [{}] has no skill group [{}]", _id, static_cast<uint32>(groupId)));
		return;
	}

	uint32 level = it->second;
	const FSkillData* skill = PlayerSkillTable::GetInst().GetSkill(static_cast<uint32>(groupId), level);
	if (!skill)
	{
		LOG(Warning, std::format("Player [{}] has invalid skill data for group [{}] level [{}]", _id, static_cast<uint32>(groupId), level));
		return;
	}
	LOG(std::format("Use Skill - {}", static_cast<uint8>(groupId)));
	auto pkt = PlayerUseSkillPacket(_id, groupId);
	ExecuteSkillEffect(*skill);
	SessionManager::GetInst().SendPacket(_id, &pkt);
	SessionManager::GetInst().BroadcastToViewList(&pkt, _id);
}

void Player::ExecuteSkillEffect(const FSkillData& skill)
{
	if (skill.Type0 == ESkillType::Atk)
	{
		float prevDmg = _stats.Damage;
		float prevAtkR = _info.AttackRadius;
		float prevFov = _info.fovAngle;

		_stats.Damage += skill.Value0;

		switch (skill.Type1)
		{
		case ESkillType::Dash:
			_info.SetLocation(_info.Pos + _info.GetFrontVector() * skill.Value1);
			break;
		case ESkillType::RangeAtk:
			_info.AttackRadius += 100.f * skill.Value1;
			break;
		case ESkillType::SectorAtk:
			_info.fovAngle += skill.Value1;
			break;
		default:
			break;
		}
		GameWorld::GetInst().PlayerAttack(_id);
		_stats.Damage = prevDmg;
		_info.AttackRadius = prevAtkR;
		_info.fovAngle = prevFov;
	}
	else if (skill.Type0 == ESkillType::BuffTime)
	{
		switch (skill.Type1)
		{
		case ESkillType::AtkSpd:
			// 공격속도 n% 증가 (클라에서 처리하자
			break;
		}

	}
}

void Player::LearnSkill(ESkillGroup groupId)
{
	if (_skillLevels.contains(groupId)) return;
	_skillLevels[groupId] = 1;
	auto pkt = SkillUnlockPacket(groupId);
	SessionManager::GetInst().SendPacket(_id, &pkt);
}

void Player::UpgradeSkill(ESkillGroup groupId)
{
	auto& lv = _skillLevels[groupId];
	if (lv < 3)
		lv++;
}

void Player::UnlockSkillsOnLevelUp()
{
	if (_stats.Level < 2)
		return;

	int32 order = _stats.Level - 2;

	int32 baseGroup = 0;
	switch (_playerType)
	{
	case Type::EPlayer::WARRIOR:
		baseGroup = (int32)ESkillGroup::HitHard;
		break;
	case Type::EPlayer::GUNNER:
		baseGroup = (int32)ESkillGroup::Throwing;
		break;
	default:
		LOG(Warning, "Unknown Player Type");
		return;
	}

	int32 skillGroup = baseGroup + (order % 3);
	int32 skillLevel = (order / 3) + 1;

	const FSkillData* skill = PlayerSkillTable::GetInst().GetSkill(skillGroup, skillLevel);
	if (skill == nullptr)
	{
		LOG(Warning, "Invaild!");
		return;
	}
	ESkillGroup groupId = static_cast<ESkillGroup>(skill->SkillGroup);

	if (!_skillLevels.contains(groupId))
	{
		LearnSkill(groupId);
		LOG(Log, std::format("Learned New Skill [{}] Level [{}]", static_cast<uint32>(groupId), skillLevel));
	}
	else
	{
		UpgradeSkill(groupId);
		LOG(Log, std::format("Upgraded Skill [{}] to Level [{}]", static_cast<uint32>(groupId), _skillLevels[groupId]));
	}
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
	auto pkt = ItemPkt::ItemUsedPacket(itemId, _info);
	SessionManager::GetInst().SendPacket(_id, &pkt);
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
	UnlockSkillsOnLevelUp();
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
