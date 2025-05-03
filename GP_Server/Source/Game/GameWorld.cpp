#include "pch.h"
#include "GameWorld.h"

bool GameWorld::Init()
{
	bool res =
		ItemTable::GetInst().LoadFromCSV("../DataTable/ItemTable.csv") &&
		PlayerLevelTable::GetInst().LoadFromCSV("../DataTable/PlayerLevelTable.csv") &&
		PlayerSkillTable::GetInst().LoadFromCSV("../DataTable/PlayerSkillTable.csv") &&
		MonsterTable::GetInst().LoadFromCSV("../DataTable/MonsterTable.csv");
	if (!res)
	{
		LOG(LogType::Warning, "LoadFromCSV");
		return false;
	}

	CreateMonster();

	return true;
}

std::shared_ptr<Player> GameWorld::GetPlayerByID(int32 id)
{
	if (id < 0 || id >= MAX_PLAYER) return nullptr;
	std::unique_lock lock(_playerMutex);
	return _players[id];
}

std::shared_ptr<Monster> GameWorld::GetMonsterByID(int32 id)
{
	std::lock_guard lock(_monsterMutex);
	auto it = _monsters.find(id);
	if (it != _monsters.end()) return it->second;
	return nullptr;
}

std::shared_ptr<Character> GameWorld::GetCharacterByID(int32 id)
{
	if (id < MAX_PLAYER) return GetPlayerByID(id);
	return GetMonsterByID(id);
}

FInfoData& GameWorld::GetInfo(int32 id)
{
	return GetCharacterByID(id)->GetInfo();
}

bool GameWorld::IsMonster(int32 id)
{
	return id >= MAX_PLAYER;
}

void GameWorld::PlayerEnterGame(std::shared_ptr<Player> player)
{
	{
		std::lock_guard lock(_playerMutex);
		int32 id = player->GetInfo().ID;
		_players[id] = player;
	}
	UpdateViewList(player);
}

void GameWorld::PlayerLeaveGame(int32 id)
{
	RemoveCharacter(id);
}

void GameWorld::RemoveCharacter(int32 id)
{
	if (id < MAX_PLAYER)
	{
		std::lock_guard<std::mutex> lock(_playerMutex);
		auto player = _players[id];
		if (!player) return;
		auto pkt = InfoPacket(EPacketType::S_REMOVE_PLAYER, player->GetInfo());
		std::unordered_set<int32> viewList;
		{
			std::lock_guard lock(player->_vlLock);
			viewList = player->GetViewList();
		}
		SessionManager::GetInst().BroadcastToViewList(&pkt, viewList);

		_players[id] = nullptr;
		for (auto& p : _players)
			if (p) { p->RemoveFromViewList(id); }

		{
			std::lock_guard mlock(_monsterMutex);
			for (auto& [mid, m] : _monsters)
				if (m) { m->RemoveFromViewList(id); }
		}
	}
	else
	{
		std::lock_guard<std::mutex> lock(_monsterMutex);
		auto it = _monsters.find(id);
		if (it == _monsters.end()) return;
		auto monster = it->second;
		auto pkt = InfoPacket(EPacketType::S_REMOVE_MONSTER, monster->GetInfo());
		std::unordered_set<int32> viewList;
		{
			std::lock_guard lock(monster->_vlLock);
			viewList = monster->GetViewList();
		}
		SessionManager::GetInst().BroadcastToViewList(&pkt, viewList);
		_monsters.erase(it);
		{
			std::lock_guard plock(_playerMutex);
			for (auto& p : _players)
				if (p) { p->RemoveFromViewList(id); }
		}
	}
}

void GameWorld::PlayerSetYaw(int32 playerId, float yaw)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invaild!");
		return;
	}
	player->GetInfo().SetYaw(yaw);
}

void GameWorld::PlayerAddState(int32 playerId, ECharacterStateType newState)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invaild!");
		return;
	}
	player->ChangeState(newState);
	auto upkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
	std::unordered_set<int32> viewList;
	{
		std::lock_guard lock(player->_vlLock);
		viewList = player->GetViewList();
	}
	SessionManager::GetInst().BroadcastToViewList(&upkt, viewList);
}

void GameWorld::PlayerRemoveState(int32 playerId, ECharacterStateType oldState)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invaild!");
		return;
	}
	player->RemoveState(oldState);
	auto upkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
	std::unordered_set<int32> viewList;
	{
		std::lock_guard lock(player->_vlLock);
		viewList = player->GetViewList();
	}
	SessionManager::GetInst().BroadcastToViewList(&upkt, viewList);
}

void GameWorld::PlayerMove(int32 playerId, FVector& pos, uint32 state, uint64& time)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invaild");
		return;
	}
	LOG(std::format("Player [{}] Move {}", playerId, pos.ToString()));
	player->GetInfo().SetLocationAndYaw(pos);
	player->GetInfo().State = static_cast<ECharacterStateType>(state);
	UpdateViewList(player);
	auto pkt = MovePacket(playerId, pos, state, time, EPacketType::S_PLAYER_MOVE);
	SessionManager::GetInst().SendPacket(playerId, &pkt);
	auto upkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
	std::unordered_set<int32> viewList;
	{
		std::lock_guard lock(player->_vlLock);
		viewList = player->GetViewList();
	}
	SessionManager::GetInst().BroadcastToViewList(&upkt, viewList);
}

void GameWorld::PlayerAttack(int32 playerId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invaild!");
		return;
	}

	for (int32 targetId : player->GetViewList())
	{
		if (!IsMonster(targetId)) continue;
		auto monster = GetMonsterByID(targetId);
		if (!monster) continue;
		if (!player->Attack(monster)) continue;
		if (monster->IsDead())
		{
			player->AddExp(10 * monster->GetInfo().GetLevel());
			SpawnWorldItem({ monster->GetInfo().Pos.X, monster->GetInfo().Pos.Y, monster->GetInfo().Pos.Z + 20 });
			RemoveCharacter(targetId);
		}
	}

	auto infopkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
	SessionManager::GetInst().SendPacket(playerId, &infopkt);
	std::unordered_set<int32> viewList;
	{
		std::lock_guard lock(player->_vlLock);
		viewList = player->GetViewList();
	}
	SessionManager::GetInst().BroadcastToViewList(&infopkt, viewList);
}

void GameWorld::PlayerUseSkill(int32 playerId, ESkillGroup groupId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invaild!");
		return;
	}
	player->UseSkill(groupId);
}

void GameWorld::CreateMonster()
{
	for (int i = 0; i < MAX_MONSTER; ++i)
	{
		int32 id = MAX_PLAYER + i;
		auto monster = std::make_shared<Monster>(id);
		{
			std::lock_guard lock(_monsterMutex);
			_monsters[id] = monster;
		}
	}
	TimerQueue::AddTimer([] { GameWorld::GetInst().UpdateMonster(); }, 2000, true);
}

void GameWorld::UpdateMonster()
{
	LOG("Update Monster");
	{
		std::lock_guard lock(_monsterMutex);
		for (auto& [id, monster] : _monsters)
		{
			if (monster) monster->Update();
		}
	}
	BroadcastMonsterStates();
}

void GameWorld::BroadcastMonsterStates()
{
	struct Snap { int id; FInfoData info; };
	std::vector<Snap> snaps;
	{
		std::lock_guard lock(_monsterMutex);
		for (auto& [id, m] : _monsters)
			if (m)
				snaps.push_back({ id, m->GetInfo() });
	}

	for (auto& s : snaps)
	{
		auto owner = GameWorld::GetInst().GetMonsterByID(s.id);
		std::unordered_set<int32> viewList;
		{
			std::lock_guard lock(owner->_vlLock);
			viewList = owner->GetViewList();
		}

		InfoPacket pkt(S_MONSTER_STATUS_UPDATE, s.info);
		SessionManager::GetInst().BroadcastToViewList(&pkt, viewList);
	}
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
	ItemPkt::SpawnPacket packet(newItem->GetItemID(), newItem->GetItemTypeID(), position);
	_worldItems.emplace_back(newItem);
	SessionManager::GetInst().BroadcastToAll(&packet);
}

void GameWorld::SpawnWorldItem(WorldItem dropedItem)
{
	std::lock_guard<std::mutex> lock(_iMutex);
	auto newItem = std::make_shared<WorldItem>(dropedItem);
	_worldItems.emplace_back(newItem);
	ItemPkt::DropPacket packet(newItem->GetItemID(), newItem->GetItemTypeID(), newItem->GetPos());
	SessionManager::GetInst().BroadcastToAll(&packet);
}

void GameWorld::PickUpWorldItem(int32 playerId, uint32 itemId)
{
	std::lock_guard<std::mutex> lock(_iMutex);
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invalid player in PickUpWorldItem");
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
		auto pkt = ItemPkt::AddInventoryPacket(targetItem->GetItemID(), targetItem->GetItemTypeID());
		SessionManager::GetInst().SendPacket(playerId, &pkt);
		auto pkt1 = ItemPkt::PickUpPacket(itemId);
		SessionManager::GetInst().BroadcastToAll(&pkt1);
	}
	else
	{
		LOG(Warning, "Failed TakeItem");
	}
}

void GameWorld::DropInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = GetPlayerByID(playerId);
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
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invalid");
		return;
	}
	player->UseItem(itemId);
}

void GameWorld::EquipInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invalid");
		return;
	}
	uint8 itemTypeID = player->EquipItem(itemId);
	std::unordered_set<int32> viewList;
	{
		std::lock_guard lock(player->_vlLock);
		viewList = player->GetViewList();
	}

	auto pkt1 = ItemPkt::EquipItemPacket(playerId, itemTypeID, player->GetStats());
	SessionManager::GetInst().BroadcastToViewList(&pkt1, viewList);
}

void GameWorld::UnequipInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invalid");
		return;
	}
	uint8 itemTypeID = player->UnequipItem(itemId);
	std::unordered_set<int32> viewList;
	{
		std::lock_guard lock(player->_vlLock);
		viewList = player->GetViewList();
	}
	auto pkt1 = ItemPkt::UnequipItemPacket(playerId, itemTypeID, player->GetStats());
	SessionManager::GetInst().BroadcastToViewList(&pkt1, viewList);
}

FVector GameWorld::TransferToZone(int32 playerId, ZoneType targetZone)
{
	return FVector();
}

FVector GameWorld::RespawnPlayer(int32 playerId, ZoneType targetZone)
{
	return FVector();
}

void GameWorld::UpdateViewList(std::shared_ptr<Character> listOwner)
{
	int32 ownerId = listOwner->GetInfo().ID;
	if (IsMonster(ownerId))
	{
		std::lock_guard lock(_playerMutex);
		for (const auto& player : _players)
		{
			if (player)
				listOwner->UpdateViewList(player);
		}
	}
	else
	{
		{
			std::lock_guard lock1(_playerMutex);
			for (const auto& player : _players)
			{
				if (player && player->GetInfo().ID != ownerId)
					listOwner->UpdateViewList(player);
			}
		}
		{
			std::lock_guard lock2(_monsterMutex);
			for (const auto& [id, monster] : _monsters)
			{
				if (monster)
					listOwner->UpdateViewList(monster);
			}
		}
	}
}

bool GameWorld::IsCollisionDetected(const FVector& pos)
{
	const float margin = 10.f;
	std::lock_guard lock1(_playerMutex);
	{
		for (const auto& player : _players)
		{
			if (player && player->IsCollision(pos, margin))
				return true;
		}
	}

	std::lock_guard lock2(_monsterMutex);
	{
		for (const auto& [id, monster] : _monsters)
		{
			if (monster && monster->IsCollision(pos, margin))
				return true;
		}
	}
	return false;
}

bool GameWorld::IsCollisionDetected(const FInfoData& target)
{
	{
		std::lock_guard lock1(_playerMutex);
		for (const auto& player : _players)
		{
			if (player && player->IsCollision(target))
				return true;
		}
	}

	{
		std::lock_guard lock2(_monsterMutex);
		for (const auto& [id, monster] : _monsters)
		{
			if (monster && monster->IsCollision(target))
				return true;
		}
	}
	return false;
}
