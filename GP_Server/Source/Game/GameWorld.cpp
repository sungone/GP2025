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
	TimerQueue::AddTimer([] { GameWorld::GetInst().UpdateAllMonsters(); }, 2000, true);

	return true;
}

std::shared_ptr<Player> GameWorld::GetPlayerByID(int32 id)
{
	if (id < 0 || id >= MAX_PLAYER) return nullptr;
	std::unique_lock lock(_mtPlayers);
	return _players[id];
}

std::shared_ptr<Monster> GameWorld::GetMonsterByID(int32 id)
{
	std::lock_guard lock(_mtMonZMap);
	for (auto& [zone, zoneMap] : _monstersByZone)
	{
		auto it = zoneMap.find(id);
		if (it != zoneMap.end())
			return it->second;
	}
	return nullptr;
}

std::shared_ptr<Character> GameWorld::GetCharacterByID(int32 id)
{
	if (id < MAX_PLAYER) return GetPlayerByID(id);
	return GetMonsterByID(id);
}

bool GameWorld::IsMonster(int32 id)
{
	return id >= MAX_PLAYER;
}

void GameWorld::PlayerEnterGame(std::shared_ptr<Player> player)
{
	FVector newPos;
	ZoneType startZone = ZoneType::TUK;
	float radius = playerCollision;
	do {
		newPos = Map::GetInst().GetRandomPos(startZone, radius);
	} while (GameWorld::GetInst().IsCollisionDetected(startZone, newPos, radius));
	player->SetPos(newPos);

	int32 id = player->GetInfo().ID;
	player->GetInfo().SetZone(startZone);

	{
		std::lock_guard lock(_mtPlayers);
		_players[id] = player;
	}

	{
		std::lock_guard lock(_mtPlayerZMap);
		_playersByZone[startZone][id] = player;
	}

	UpdateViewList(player);
}

void GameWorld::PlayerLeaveGame(int32 id)
{
	std::shared_ptr<Player> player;
	std::unordered_set<int32> viewList;
	ZoneType zone;
	InfoPacket pkt(InfoPacket(EPacketType::S_REMOVE_PLAYER, {}));

	std::lock_guard<std::mutex> lock(_mtPlayers);
	{
		player = _players[id];
		if (!player) return;
		zone = player->GetZone();
		pkt = InfoPacket(EPacketType::S_REMOVE_PLAYER, player->GetInfo());
		{
			std::lock_guard lock(player->_vlLock);
			viewList = player->GetViewList();
		}

		_players[id] = nullptr;
	}
	SessionManager::GetInst().BroadcastToViewList(&pkt, viewList);

	{
		std::lock_guard lock(_mtPlayerZMap);
		auto& players = _playersByZone[zone];
		for (auto& [pid, p] : players)
			if (p) p->RemoveFromViewList(id);
		players.erase(id);
		if (players.empty())
			_playersByZone.erase(zone);
	}
	{
		std::lock_guard<std::mutex> lock(_mtMonZMap);
		auto& monsters = _monstersByZone[zone];
		{
			for (auto& [mid, m] : monsters)
			{
				if (m) m->RemoveFromViewList(id);
			}
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
	std::unordered_set<int32> viewList;
	{
		std::lock_guard lock(player->_vlLock);
		viewList = player->GetViewList();
	}
	for (int32 targetId : viewList)
	{
		if (!IsMonster(targetId)) continue;
		auto monster = GetMonsterByID(targetId);
		if (!monster) continue;
		if (!player->Attack(monster)) continue;
		if (monster->IsDead())
		{
			uint32 monlv = monster->GetInfo().GetLevel();
			Type::EPlayer playertype = (Type::EPlayer)player->GetInfo().CharacterType;
			player->AddExp(10 * monster->GetInfo().GetLevel());
			FVector itemPos = { monster->GetInfo().Pos.X, monster->GetInfo().Pos.Y, monster->GetInfo().Pos.Z + 20 };
			SpawnWorldItem(itemPos, monlv, playertype);
			RemoveMonster(targetId);
		}
	}

	auto infopkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
	SessionManager::GetInst().SendPacket(playerId, &infopkt);
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
	for (auto const& tpl : _spawnTable)
	{
		auto [zone, typeId, count] = tpl;
		auto& zoneMap = _monstersByZone[zone];

		for (int i = 0; i < count; ++i)
		{
			int32 id = GenerateMonsterId();
			auto monster = std::make_shared<Monster>(id, zone, typeId);
			monster->Init();
			{
				std::lock_guard lock(_mtMonZMap);
				zoneMap[id] = monster;
			}
		}
	}
}

void GameWorld::RemoveMonster(int32 id)
{
	std::lock_guard<std::mutex> lock(_mtMonZMap);
	for (auto& [zone, zoneMap] : _monstersByZone)
	{
		auto it = zoneMap.find(id);
		if (it == zoneMap.end()) continue;

		auto monster = it->second;
		InfoPacket pkt(EPacketType::S_REMOVE_MONSTER, monster->GetInfo());
		std::unordered_set<int32> viewList;
		{
			std::lock_guard lock(monster->_vlLock);
			viewList = monster->GetViewList();
		}
		SessionManager::GetInst().BroadcastToViewList(&pkt, viewList);

		zoneMap.erase(it);
		break;
	}
	{
		std::lock_guard plock(_mtPlayers);
		for (auto& p : _players)
			if (p) { p->RemoveFromViewList(id); }
	}
}

void GameWorld::UpdateAllMonsters()
{
	std::vector<std::pair<int32, FInfoData>> snaps;
	{
		std::lock_guard lock(_mtMonZMap);
		for (auto& [zone, zoneMap] : _monstersByZone)
		{
			for (auto& [id, monster] : zoneMap)
			{
				if (!monster) continue;
				monster->Update();
				snaps.emplace_back(id, monster->GetInfo());
			}
		}
	}

	for (auto& [id, info] : snaps)
	{
		InfoPacket pkt(EPacketType::S_MONSTER_STATUS_UPDATE, info);

		auto monster = GetMonsterByID(id);
		if (!monster) continue;
		std::unordered_set<int32> viewList;
		{
			std::lock_guard lock(monster->_vlLock);
			viewList = monster->GetViewList();
		}

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

void GameWorld::SpawnWorldItem(FVector position, uint32 monlv, Type::EPlayer playertype)
{
	std::lock_guard<std::mutex> lock(_mtItem);
	auto newItem = std::make_shared<WorldItem>(position, monlv, playertype);

	ItemPkt::SpawnPacket packet(newItem->GetItemID(), newItem->GetItemTypeID(), position);
	_worldItems.emplace_back(newItem);
	SessionManager::GetInst().BroadcastToAll(&packet);
}

void GameWorld::SpawnWorldItem(WorldItem dropedItem)
{
	std::lock_guard<std::mutex> lock(_mtItem);
	auto newItem = std::make_shared<WorldItem>(dropedItem);
	_worldItems.emplace_back(newItem);
	ItemPkt::DropPacket packet(newItem->GetItemID(), newItem->GetItemTypeID(), newItem->GetPos());
	SessionManager::GetInst().BroadcastToAll(&packet);
}

void GameWorld::PickUpWorldItem(int32 playerId, uint32 itemId)
{
	std::lock_guard<std::mutex> lock(_mtItem);
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

	auto pkt = ItemPkt::EquipItemPacket(playerId, itemTypeID, player->GetInfo());
	SessionManager::GetInst().SendPacket(playerId, &pkt);
	SessionManager::GetInst().BroadcastToViewList(&pkt, viewList);
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
	auto pkt = ItemPkt::UnequipItemPacket(playerId, itemTypeID, player->GetInfo());
	SessionManager::GetInst().SendPacket(playerId, &pkt);
	SessionManager::GetInst().BroadcastToViewList(&pkt, viewList);
}

FVector GameWorld::TransferToZone(int32 playerId, ZoneType targetZone)
{
	auto player = GetPlayerByID(playerId);
	if (!player) return FVector();

	ZoneType oldZone = player->GetZone();
	float radius = playerCollision;
	//Todo: 랜덤스폰이 아니라 입구 쪽 스폰으로 설정해줘야한다
	FVector newPos;
	do {
		newPos = Map::GetInst().GetRandomPos(targetZone, radius);
	} while (IsCollisionDetected(targetZone, newPos, radius));

	player->GetInfo().SetLocation(newPos);
	player->GetInfo().SetZone(targetZone);
	std::unordered_set<int32> oldvlist;
	{
		std::lock_guard lock(player->_vlLock);
		oldvlist = player->GetViewList();
	}
	for (int32 mid : oldvlist)
	{
		auto other = GetCharacterByID(mid);
		if (!other) continue;
		if (other->IsMonster())
		{
			player->RemoveMonsterFromViewList(other);
		}
		else
		{
			player->RemovePlayerFromViewList(other);
		}
	}
	{
		std::lock_guard lock(_mtPlayerZMap);
		auto& oldMap = _playersByZone[oldZone];
		oldMap.erase(playerId);
		if (oldMap.empty()) _playersByZone.erase(oldZone);
		_playersByZone[targetZone][playerId] = player;
	}

	UpdateViewList(player);
	return newPos;
}

void GameWorld::RespawnPlayer(int32 playerId, ZoneType targetZone)
{
	FVector respawnPos = TransferToZone(playerId, targetZone);

	auto player = GetPlayerByID(playerId);
	if (player) {
		auto& info = player->GetInfo();
		info.Stats.Hp = info.Stats.MaxHp;
		info.State = ECharacterStateType::STATE_IDLE;

		RespawnPacket pkt(info);
		SessionManager::GetInst().SendPacket(playerId, &pkt);
	}
}

void GameWorld::UpdateViewList(std::shared_ptr<Character> listOwner)
{
	int32 ownerId = listOwner->GetInfo().ID;
	ZoneType ownerZone = listOwner->GetZone();
	if (IsMonster(ownerId))
	{
		std::lock_guard lock(_mtPlayerZMap);
		for (auto& [pid, player] : _playersByZone[ownerZone])
		{
			if (player)
				listOwner->UpdateViewList(player);
		}
	}
	else
	{
		{
			std::lock_guard lock(_mtPlayerZMap);
			for (auto& [pid, player] : _playersByZone[ownerZone])
			{
				if (player && pid != ownerId)
					listOwner->UpdateViewList(player);
			}
		}
		{
			std::lock_guard lock(_mtMonZMap);
			for (auto& [mid, monster] : _monstersByZone[ownerZone])
			{
				if (monster)
					listOwner->UpdateViewList(monster);
			}
		}
	}
}

bool GameWorld::IsCollisionDetected(ZoneType zone, const FVector& pos, float dist)
{
	{
		std::lock_guard lock(_mtPlayerZMap);
		for (auto& [pid, player] : _playersByZone[zone])
		{
			if (player && player->IsCollision(pos, dist))
				return true;
		}
	}
	{
		std::lock_guard lock(_mtMonZMap);
		for (auto& [mid, monster] : _monstersByZone[zone])
		{
			if (monster && monster->IsCollision(pos, dist))
				return true;
		}
	}
	return false;
}
