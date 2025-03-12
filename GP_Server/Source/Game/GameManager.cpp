#include "pch.h"
#include "GameManager.h"

bool GameManager::Init()
{
	CreateMonster();
	StartMonsterStateBroadcast();
	UpdateMonster();
	return true;
}

void GameManager::AddPlayer(std::shared_ptr<Character> player)
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	int32 id = player->GetInfo().ID;
	_characters[id] = player;

}

void GameManager::RemoveCharacter(int32 id)
{
	LOG(Log, std::format("Remove Character"));
	if (id < 0 || id >= MAX_CHARACTER || !_characters[id])
	{
		LOG(Warning, "Invalid");
		return;
	}

	if (id < MAX_PLAYER)
	{
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_PLAYER, _characters[id]->GetInfo());
		SessionManager::GetInst().Broadcast(&Pkt);
	}
	else
	{
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_MONSTER, _characters[id]->GetInfo());
		SessionManager::GetInst().Broadcast(&Pkt);
	}

	_characters[id] = nullptr;
}

void GameManager::CreateMonster()
{
	for (int32 i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		_characters[i] = std::make_shared<Monster>();
		_characters[i]->Init();
		_characters[i]->GetInfo().ID = i;
	}
}

void GameManager::SpawnMonster(Session& session)
{
	std::lock_guard<std::mutex> lock(_carrMutex);

	for (int32 i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (_characters[i] && _characters[i]->IsValid())
		{
			auto Pkt = InfoPacket(EPacketType::S_ADD_MONSTER, _characters[i]->GetInfo());
			session.DoSend(&Pkt);
		}
	}
}

void GameManager::ProcessAttack(int32 attackerID, int32 targetID)
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	if (targetID == -1)
		return;

	LOG(Log, std::format("Attacked monster[{}]", targetID));

	auto& Attacker = _characters[attackerID];
	std::shared_ptr<Monster> Target = static_pointer_cast<Monster>(_characters[targetID]);

	if (!CollisionUtils::CanAttack(Attacker->GetInfo(), Target->GetInfo()))
		return;

	float atkDamage = Attacker->GetAttackDamage();
	if (atkDamage > 0.0f)
		Target->OnDamaged(atkDamage);

	auto pkt = DamagePacket(Target->GetInfo(), atkDamage);
	SessionManager::GetInst().Broadcast(&pkt);
	if (Target->IsDead())
	{
		SpawnWorldItem({ Target->GetInfo().Pos.X,Target->GetInfo().Pos.Y,Target->GetInfo().Pos.Z + 120 });
		RemoveCharacter(targetID);
	}
}

std::shared_ptr<Character> GameManager::GetCharacterByID(int32 id)
{
	std::lock_guard<std::mutex> lock(_carrMutex);

	if (id < 0 || id >= MAX_CHARACTER || !_characters[id] || !_characters[id]->IsValid())
	{
		LOG(Warning, "Invalid");
		return nullptr;
	}
	return _characters[id];
}

void GameManager::StartMonsterStateBroadcast()
{
	_MonsterStateBroadcastTimer.Start(3000, [this]() {
		BroadcastMonsterStates();
		});
}

void GameManager::BroadcastMonsterStates()
{
	std::lock_guard<std::mutex> lock(_carrMutex);
	LOG(SendLog, std::format("Broadcast monster"));

	for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (_characters[i] && _characters[i]->IsValid())
		{
			auto& monster = _characters[i];
			FInfoData MonsterInfoData = monster->GetInfo();
			InfoPacket packet(S_MONSTER_STATUS_UPDATE, MonsterInfoData);
			SessionManager::GetInst().Broadcast(&packet);
		}
	}
}

void GameManager::UpdateMonster()
{
	_MonsterAIUpdateTimer.Start(4000, [this]() {
		std::unique_lock<std::mutex> lock(_carrMutex);
		for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
		{
			if (!_characters[i]) return;

			_characters[i]->Update();
		}
		});
}

bool GameManager::RemoveWorldItem(std::shared_ptr<WorldItem> item)
{
	if (!item) return false;

	auto it = std::remove_if(_worldItems.begin(), _worldItems.end(),
		[&item](const std::shared_ptr<WorldItem>& worldItem) {
			return worldItem == item;
		});

	if (it != _worldItems.end())
	{
		_worldItems.erase(it, _worldItems.end());
		return true;
	}
	return false;
}

std::shared_ptr<WorldItem> GameManager::FindWorldItemById(uint32 itemId)
{
	auto it = std::find_if(_worldItems.begin(), _worldItems.end(),
		[itemId](const std::shared_ptr<WorldItem>& item) {
			return item->GetItemID() == itemId;
		});

	if (it != _worldItems.end())
	{
		return *it;
	}
	return nullptr;
}

void GameManager::SpawnWorldItem(FVector position)
{
	std::lock_guard<std::mutex> lock(_iMutex);
	auto newItem = std::make_shared<WorldItem>(position);
	_worldItems.emplace_back(newItem);
	ItemPkt::SpawnPacket packet(newItem->GetItemID(), newItem->GetRandomItemType(), position);
	SessionManager::GetInst().Broadcast(&packet);
}

void GameManager::SpawnWorldItem(WorldItem dropedItem)
{
	std::lock_guard<std::mutex> lock(_iMutex);
	auto newItem = std::make_shared<WorldItem>(dropedItem);
	_worldItems.emplace_back(newItem);
	ItemPkt::DropPacket packet(newItem->GetItemID(), newItem->GetRandomItemType(), newItem->GetPos());
	SessionManager::GetInst().Broadcast(&packet);
}

void GameManager::PickUpWorldItem(int32 playerId, uint32 itemId)
{
	std::lock_guard<std::mutex> lock(_iMutex);

	if (playerId < 0 || playerId >= MAX_PLAYER || !_characters[playerId])
	{
		LOG(Warning, "Invalid");
		return;
	}

	auto player = std::dynamic_pointer_cast<Player>(_characters[playerId]);
	if (!player)
	{
		LOG(Warning, "Invalid");
		return;
	}

	auto targetItem = FindWorldItemById(itemId);
	if (!targetItem)
	{
		LOG(Warning, "Invalid");
		return;
	}

	if (player->TakeWorldItem(targetItem))
	{
		RemoveWorldItem(targetItem);
		auto pkt = ItemPkt::AddInventoryPacket(targetItem->GetItemID(), targetItem->GetItemType());
		SessionManager::GetInst().SendPacket(playerId, &pkt);
		auto pkt1 = ItemPkt::PickUpPacket(itemId);
		SessionManager::GetInst().Broadcast(&pkt1);
	}
	else
	{
		LOG(Warning, "Failed TakeItem");
	}
}

void GameManager::DropInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = std::dynamic_pointer_cast<Player>(_characters[playerId]);
	if (!player)
	{
		LOG(Warning, "Invalid");
		return;
	}

	WorldItem dropedItem = player->DropItem(itemId);
	SpawnWorldItem(dropedItem);
}

void GameManager::UseInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = std::dynamic_pointer_cast<Player>(_characters[playerId]);
	if (!player)
	{
		LOG(Warning, "Invalid");
		return;
	}
	player->UseItem(itemId);
}

void GameManager::EquipInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = std::dynamic_pointer_cast<Player>(_characters[playerId]);
	if (!player)
	{
		LOG(Warning, "Invalid");
		return;
	}
	uint8 itemType = player->EquipItem(itemId);
	auto pkt1 = ItemPkt::EquipItemPacket(playerId, itemType);
	SessionManager::GetInst().Broadcast(&pkt1);
}

void GameManager::UnequipInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = std::dynamic_pointer_cast<Player>(_characters[playerId]);
	if (!player)
	{
		LOG(Warning, "Invalid");
		return;
	}
	uint8 itemType = player->UnequipItem(itemId);
	auto pkt1 = ItemPkt::UnequipItemPacket(playerId, itemType);
	SessionManager::GetInst().Broadcast(&pkt1);
}