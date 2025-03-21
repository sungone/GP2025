#include "pch.h"
#include "GameWorld.h"

bool GameWorld::Init()
{
	CreateMonster();
	return true;
}

void GameWorld::AddPlayer(std::shared_ptr<Character> player)
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	int32 id = player->GetInfo().ID;
	_characters[id] = player;

}

void GameWorld::RemoveCharacter(int32 id)
{
	if (id < 0 || id >= MAX_CHARACTER || !_characters[id])
	{
		LOG(Warning, "Invalid");
		return;
	}

	if (id < MAX_PLAYER)
	{
		LOG(Log, std::format("Remove Player"));
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_PLAYER, _characters[id]->GetInfo());
		SessionManager::GetInst().Broadcast(&Pkt);
	}
	else
	{
		LOG(Log, std::format("Remove Monster"));
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_MONSTER, _characters[id]->GetInfo());
		SessionManager::GetInst().Broadcast(&Pkt);
	}

	_characters[id] = nullptr;
}

void GameWorld::CreateMonster()
{
	for (int32 i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		_characters[i] = std::make_shared<Monster>();
		_characters[i]->Init();
		_characters[i]->GetInfo().ID = i;
	}
	TimerQueue::AddTimerEvent(TimerEvent(0, ::AI_Patrol, 2000));
}

void GameWorld::SpawnMonster(Session& session)
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

void GameWorld::PlayerMove(int32 playerId, FInfoData& info)
{
	_characters[playerId]->SetInfo(info);
	LOG(std::format("Player[{}] Move to {}", playerId, info.Pos.ToString()));
	auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, info);
	SessionManager::GetInst().Broadcast(&pkt, playerId);
}

void GameWorld::PlayerAttack(int32 attackerID, int32 targetID)
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	auto& Attacker = _characters[attackerID];
	auto& atkInfo = Attacker->GetInfo();
	atkInfo.AddState(ECharacterStateType::STATE_AUTOATTACK);
	auto infopkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, atkInfo);
	SessionManager::GetInst().Broadcast(&infopkt);

	if (targetID == -1)
		return;

	LOG(Log, std::format("Attacked monster[{}]", targetID));

	std::shared_ptr<Monster> Target = static_pointer_cast<Monster>(_characters[targetID]);

	if (!Attacker->IsInAttackRange(Target->GetInfo()))
		return;

#ifdef _DEBUG
	float atkDamage = 50;
#else
	float atkDamage = Attacker->GetAttackDamage();
#endif // TEST
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

std::shared_ptr<Character> GameWorld::GetCharacterByID(int32 id)
{
	std::lock_guard<std::mutex> lock(_carrMutex);

	if (id < 0 || id >= MAX_CHARACTER || !_characters[id] || !_characters[id]->IsValid())
	{
		LOG(Warning, "Invalid");
		return nullptr;
	}
	return _characters[id];
}

void GameWorld::BroadcastMonsterStates()
{
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

void GameWorld::UpdateMonster()
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (!_characters[i]) return;

		_characters[i]->Update();
	}
	BroadcastMonsterStates();
}

bool GameWorld::RemoveWorldItem(std::shared_ptr<WorldItem> item)
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

std::shared_ptr<WorldItem> GameWorld::FindWorldItemById(uint32 itemId)
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

void GameWorld::SpawnWorldItem(FVector position)
{
	std::lock_guard<std::mutex> lock(_iMutex);
	auto newItem = std::make_shared<WorldItem>(position);
	ItemPkt::SpawnPacket packet(newItem->GetItemID(), newItem->GetItemType(), position);
	_worldItems.emplace_back(newItem);
	SessionManager::GetInst().Broadcast(&packet);
}

void GameWorld::SpawnWorldItem(WorldItem dropedItem)
{
	std::lock_guard<std::mutex> lock(_iMutex);
	auto newItem = std::make_shared<WorldItem>(dropedItem);
	_worldItems.emplace_back(newItem);
	ItemPkt::DropPacket packet(newItem->GetItemID(), newItem->GetItemType(), newItem->GetPos());
	SessionManager::GetInst().Broadcast(&packet);
}

void GameWorld::PickUpWorldItem(int32 playerId, uint32 itemId)
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

void GameWorld::DropInventoryItem(int32 playerId, uint32 itemId)
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

void GameWorld::UseInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = std::dynamic_pointer_cast<Player>(_characters[playerId]);
	if (!player)
	{
		LOG(Warning, "Invalid");
		return;
	}
	player->UseItem(itemId);
}

void GameWorld::EquipInventoryItem(int32 playerId, uint32 itemId)
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

void GameWorld::UnequipInventoryItem(int32 playerId, uint32 itemId)
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