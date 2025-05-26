#include "pch.h"
#include "Player.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Player::Init()
{
	Character::Init();
	SetCharacterType(Type::EPlayer::WARRIOR);
#ifndef DB_LOCAL
	_info.SetName(L"플레이어");
	SetCharacterType(Type::EPlayer::WARRIOR);
	_info.Stats.Level = 1;
	_info.Stats.Speed = 200.f;
	_info.CollisionRadius = playerCollision;
	_info.State = ECharacterStateType::STATE_IDLE;
	ApplyLevelStats(_info.Stats.Level);
#endif
}

void Player::LoadFromDB(const DBLoginResult& dbRes)
{
	SetInfo(dbRes.info);
	for (auto& [itemID, itemTypeID] : dbRes.items)
	{
		LoadInventoryItem(std::make_shared<Item>(itemID, itemTypeID));
	}
	//Todo: 퀘스트 DB연동
}

void Player::SaveToDB(uint32 dbId)
{
	DBManager::GetInst().UpdatePlayerInfo(dbId, _info);
	_inventory.SaveToDB(dbId);
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
		//_info.Skills.Q = FSkillData(ESkillGroup::HitHard, 1);
		//_info.Skills.E = FSkillData(ESkillGroup::Clash, 1);
		//_info.Skills.R = FSkillData(ESkillGroup::Whirlwind, 1);
	}
	else
	{
		_info.fovAngle = 10;
		_info.AttackRadius = 5000;
		//_info.Skills.Q = FSkillData(ESkillGroup::Throwing, 1);
		//_info.Skills.E = FSkillData(ESkillGroup::FThrowing, 1);
		//_info.Skills.R = FSkillData(ESkillGroup::Anger, 1);
	}
}

void Player::OnEnterGame()
{
	for (const auto& [id, invItem] : _inventory.GetItems())
	{
		if (invItem && invItem->item)
		{
			auto pkt = ItemPkt::AddInventoryPacket(invItem->item->GetItemID(), invItem->item->GetItemTypeID());
			SessionManager::GetInst().SendPacket(_id, &pkt);
		}
	}
	{
		QuestType quest = GetCurrentQuest();
		const QuestData* questData = QuestTable::GetInst().GetQuest(quest);
		if (!questData)
		{
			LOG(Warning, "Invalid quest datatable");
			return;
		}
		SetCurrentQuest(questData->NextQuestID);
		auto questpkt = QuestStartPacket(questData->NextQuestID);
		SessionManager::GetInst().SendPacket(_id, &questpkt);
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

bool Player::AddInventoryItem(std::shared_ptr<Item> item)
{
	return _inventory.AddItem(item);
}

bool Player::LoadInventoryItem(std::shared_ptr<Item> item)
{
	return _inventory.LoadItem(item);
}

bool Player::BuyItem(std::shared_ptr<Item> item, uint32 price, uint16 quantity)
{
	uint32 totalPrice = price * quantity;

	if (!SpendGold(totalPrice))
	{
		LOG("Not enough gold");
		return false;
	}
	return AddInventoryItem(item);
}

bool Player::SellItem(uint32 itemId)
{
	bool bSuccess = false;
	DBResultCode resultCode = DBResultCode::SUCCESS;
	uint32 resellPrice = 0;

	auto item = _inventory.FindItem(itemId);
	if (!item)
	{
		LOG(Warning, "Invalid item");
		resultCode = DBResultCode::ITEM_NOT_FOUND;
	}
	else
	{
		auto itemType = item->GetItemTypeID();
		auto itemData = ItemTable::GetInst().GetItemByTypeId(itemType);

		if (!itemData)
		{
			LOG(Warning, "Invalid item data");
			resultCode = DBResultCode::ITEM_NOT_FOUND;
		}
		else if (!_inventory.RemoveItem(itemId))
		{
			LOG(Warning, "Failed remove item");
			resultCode = DBResultCode::ITEM_NOT_FOUND;
		}
		else
		{
			auto pkt = ItemPkt::RemoveInventoryPacket(itemId);
			SessionManager::GetInst().SendPacket(_id, &pkt);
			resellPrice = itemData->ResellPrice;
			AddGold(resellPrice);
			bSuccess = true;
		}
	}
	uint32 curgold = GetGold();
	auto respkt = SellItemResultPacket(bSuccess, resultCode, curgold);
	SessionManager::GetInst().SendPacket(_id, &respkt);
	return bSuccess;
}

bool Player::TakeWorldItem(const std::shared_ptr<WorldItem> item)
{
	float detectDist = 500.f;
	if (!IsCollision(item->GetPos(), detectDist))
		return false;
	return  AddInventoryItem(item);
}

bool Player::Attack(std::shared_ptr<Character> target)
{
	auto monster = std::dynamic_pointer_cast<Monster>(target);
	if (!monster) return false;
	if (!IsInAttackRange(monster->GetInfo()))return false;

	float atkDamage = GetAttackDamage() * TEST_ATK_WEIGHT;
	if (atkDamage > 0.0f)
	{
		monster->OnDamaged(atkDamage);
	}

	auto pkt = MonsterDamagePacket(monster->GetInfo(), atkDamage);
	SessionManager::GetInst().SendPacket(_id, &pkt);
	SessionManager::GetInst().BroadcastToViewList(&pkt, _viewList);
	return true;
}

void Player::UseSkill(ESkillGroup groupId)
{
	auto curSkill = _info.GetSkillData(groupId);
	if (curSkill == nullptr || !curSkill->IsValid())
	{
		LOG(Warning, "Invaild!");
		return;
	}

	uint32 level = curSkill->SkillLevel;
	const FSkillTableData* skilltable = PlayerSkillTable::GetInst().GetSkill(static_cast<uint32>(groupId), level);
	if (!skilltable)
	{
		LOG(Warning, "Invaild!");
		return;
	}
	LOG(std::format("Use Skill - {}", static_cast<uint8>(groupId)));
	if (groupId == ESkillGroup::HitHard || groupId == ESkillGroup::Throwing)
		_info.AddState(STATE_SKILL_Q);
	else if (groupId == ESkillGroup::Clash || groupId == ESkillGroup::FThrowing)
		_info.AddState(STATE_SKILL_E);
	else if (groupId == ESkillGroup::Whirlwind || groupId == ESkillGroup::Anger)
		_info.AddState(STATE_SKILL_R);

	auto pkt = PlayerUseSkillPacket(_id, groupId);
	SessionManager::GetInst().SendPacket(_id, &pkt);
	SessionManager::GetInst().BroadcastToViewList(&pkt, _viewList);
	ExecuteSkillEffect(*skilltable);
}

void Player::ExecuteSkillEffect(const FSkillTableData& skill)
{
	if (skill.Type0 == ESkillType::Atk)
	{
		float prevDmg = _stats.Damage;
		float prevAtkR = _info.AttackRadius;
		float prevFov = _info.fovAngle;

		float percent = skill.Value0;
		_stats.Damage += static_cast<int>(_stats.Damage * (percent / 100.0f));


		switch (skill.Type1)
		{
		case ESkillType::Dash:
			_info.SetLocation(_info.Pos + _info.GetFrontVector() * skill.Value1);
			break;
		case ESkillType::RangeAtk:
			_info.AttackRadius = 100.f * skill.Value1;
			_info.fovAngle = 360;
			break;
		case ESkillType::SectorAtk:
			_info.fovAngle += skill.Value1;
			break;
		default:
			break;
		}
		GameWorld::GetInst().PlayerAttack(_id);
		ResetSkillEffect(prevDmg, prevAtkR, prevFov);
		//TimerQueue::AddTimer([=] {
		//	if (auto player = GameWorld::GetInst().GetPlayerByID(_id)) {
		//		player->ResetSkillEffect(prevDmg, prevAtkR, prevFov);
		//	}
		//	}, 1000, true);
		// Todo: 스킬 개선
		// R스킬 여러번 보내는듯해서 타이머 여러번 호출되면 꼬인다..
	}
	else if (skill.Type0 == ESkillType::BuffTime)
	{
		switch (skill.Type1)
		{
		case ESkillType::AtkSpd:// 공격속도 n% 증가 (클라에서 처리)
			break;
		}

	}
}

void Player::ResetSkillEffect(float prevDmg, float prevAtkR, float prevFov)
{
	_stats.Damage = prevDmg;
	_info.AttackRadius = prevAtkR;
	_info.fovAngle = prevFov;
}

void Player::LearnSkill(ESkillGroup groupId)
{
	FSkillState& skills = _info.Skills;

	if (!skills.Q.IsValid())
	{
		skills.Q.SetSkill(groupId, 1);
		return;
	}
	if (!skills.E.IsValid())
	{
		skills.E.SetSkill(groupId, 1);
		return;
	}
	if (!skills.R.IsValid())
	{
		skills.R.SetSkill(groupId, 1);
		return;
	}
}

void Player::UpgradeSkill(ESkillGroup groupId)
{
	auto* skill = _info.GetSkillData(groupId);
	if (!skill)
	{
		LOG(Warning, "Trying to upgrade unlearned skill");
		return;
	}

	skill->LevelUp();
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

	const FSkillTableData* skill = PlayerSkillTable::GetInst().GetSkill(skillGroup, skillLevel);
	if (skill == nullptr)
	{
		LOG(Warning, "Invaild!");
		return;
	}
	ESkillGroup groupId = static_cast<ESkillGroup>(skill->SkillGroup);

	auto* curSkill = _info.GetSkillData(groupId);
	if (!curSkill->IsValid())
	{
		LearnSkill(groupId);
		auto pkt = SkillUnlockPacket(groupId);
		SessionManager::GetInst().SendPacket(_id, &pkt);
		LOG(Log, std::format("Learned New Skill [{}] Level [{}]", static_cast<uint32>(groupId), skillLevel));
	}
	else
	{
		UpgradeSkill(groupId);
		auto pkt = UpgradeSkillPacket(groupId);
		SessionManager::GetInst().SendPacket(_id, &pkt);
		LOG(Log, std::format("Upgraded Skill [{}] to Level [{}]", static_cast<uint32>(groupId), curSkill->SkillLevel));
	}
}


void Player::UseItem(uint32 itemId)
{
	auto targetItem = _inventory.FindItem(itemId);
	if (!targetItem) return;

	const ItemStats& stats = targetItem->GetStats();
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
	_inventory.RemoveItem(itemId);
}

uint8 Player::EquipItem(uint32 itemId)
{
	auto targetItem = _inventory.FindItem(itemId);
	if (!targetItem) return 0;

	const ItemStats& itemStats = targetItem->GetStats();
	AddItemStats(itemStats);

	uint8 itemType = targetItem->GetItemTypeID();
	_info.EquipItemByType(itemType);

	return itemType;
}

uint8 Player::UnequipItem(uint32 itemId)
{
	auto targetItem = _inventory.FindItem(itemId);
	if (!targetItem) return 0;

	const ItemStats& itemStats = targetItem->GetStats();
	RemoveItemStats(itemStats);

	uint8 itemType = targetItem->GetItemTypeID();
	_info.UnequipItemByType(itemType);

	return itemType;
}

bool Player::SetCurrentQuest(QuestType quest)
{
	if (!_info.HasQuest(quest))
	{
		int res = _info.AddQuest({ quest,EQuestStatus::InProgress });
		if (!res) return false;
	}
	_currentQuest = quest;
	return true;
}

bool Player::CompleteCurrentQuest()
{
	return _info.CompleteQuest(_currentQuest);
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
	auto pkt = PlayerLevelUpPacket(_info);
	SessionManager::GetInst().SendPacket(_id, &pkt);

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
