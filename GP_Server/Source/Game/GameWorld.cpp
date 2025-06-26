#include "pch.h"
#include "GameWorld.h"

bool GameWorld::Init()
{
	bool res =
		ItemTable::GetInst().LoadFromCSV(DataTablePath + "ItemTable.csv") &&
		PlayerLevelTable::GetInst().LoadFromCSV(DataTablePath + "PlayerLevelTable.csv") &&
		PlayerSkillTable::GetInst().LoadFromCSV(DataTablePath + "PlayerSkillTable.csv") &&
		MonsterTable::GetInst().LoadFromCSV(DataTablePath + "MonsterTable.csv") &&
		SpawnTable::GetInst().LoadFromCSV(DataTablePath + "SpawnTable.csv") &&
		QuestTable::GetInst().LoadFromCSV(DataTablePath + "QuestTable.csv");

	if (!res)
	{
		LOG(LogType::Warning, "LoadFromCSV");
		return false;
	}

	CreateMonster();
	TimerQueue::AddTimer([] { GameWorld::GetInst().UpdateAllMonsters(); }, 1500, true);

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

	newPos = Map::GetInst().GetStartPos(startZone);
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

	//for test
	player->AddGold(10000);
	//for test
	BuyItem(id, (uint8)Type::EWeapon::POSITRON, 1);
	BuyItem(id, (uint8)Type::EWeapon::ENERGY_SWORD, 1);


	auto& playerInfo = player->GetInfo();
	UpdateViewList(player);

	EnterGamePacket enterpkt(playerInfo);
	SessionManager::GetInst().SendPacket(id, &enterpkt);
	player->OnEnterGame();
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

void GameWorld::PlayerSetLocation(int32 playerId, float yaw, FVector pos)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invaild!");
		return;
	}
	player->GetInfo().SetYaw(yaw);
	player->SetPos(pos);
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
	if (player->RemoveState(oldState))
	{
		auto upkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
		std::unordered_set<int32> viewList;
		{
			std::lock_guard lock(player->_vlLock);
			viewList = player->GetViewList();
		}
		SessionManager::GetInst().BroadcastToViewList(&upkt, viewList);
	}
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
		std::shared_ptr<Monster> monster = GetMonsterByID(targetId);
		if (!monster)continue;
		if (!player->Attack(monster)) continue;
		if (monster->IsDead())
		{
			uint32 monlv = monster->GetInfo().GetLevel();
			Type::EPlayer playertype = (Type::EPlayer)player->GetInfo().CharacterType;
			player->AddExp(TEST_EXP_WEIGHT * 10 * monster->GetInfo().GetLevel());

			FVector basePos = monster->GetInfo().Pos;
			auto mquest = monster->GetQuestID();
			if (mquest != QuestType::NONE)
			{
				uint32 dropId = monster->GetDropItemId();
				if (monster->IsBoss())
				{
					if (monster->HasDropItem())
					{
						FVector itemPos = basePos + RandomUtils::GetRandomOffset();
						auto dropedItem = WorldItem(dropId, itemPos);
						SpawnWorldItem(dropedItem);
					}

					player->GiveQuestReward(mquest);
				}
				else if (mquest == QuestType::CH2_CLEAR_E_BUILDING || mquest == QuestType::CH3_CLEAR_SERVER_ROOM)
				{
					auto zone = monster->GetZone();
					_monsterCnt[zone]--;
					if (_monsterCnt[zone] == 1)
					{
						player->GiveQuestReward(mquest);
					}
				}
			}

			//todo: 아이템 드랍테이블로 스폰하자
			{
				FVector itemPos = basePos + RandomUtils::GetRandomOffset();
				SpawnWorldItem(itemPos, monlv, playertype);
				FVector goldPos = basePos + RandomUtils::GetRandomOffset();
				SpawnGoldItem(goldPos);
			}

			RemoveMonster(targetId);
		}
	}

	auto infopkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
	SessionManager::GetInst().SendPacket(playerId, &infopkt);
	SessionManager::GetInst().BroadcastToViewList(&infopkt, viewList);
	player->RemoveState(ECharacterStateType::STATE_AUTOATTACK);
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
	auto targetState = ECharacterStateType::STATE_NONE;
	if (groupId == ESkillGroup::HitHard || groupId == ESkillGroup::Throwing)
		targetState = ECharacterStateType::STATE_SKILL_Q;
	else if (groupId == ESkillGroup::Clash || groupId == ESkillGroup::FThrowing)
		targetState = ECharacterStateType::STATE_SKILL_E;
	else if (groupId == ESkillGroup::Whirlwind || groupId == ESkillGroup::Anger)
		targetState = ECharacterStateType::STATE_SKILL_R;
	player->RemoveState(targetState);
}

void GameWorld::PlayerDead(int32 playerID)
{
	auto player = GetPlayerByID(playerID);
	if (!player)
	{
		LOG(Warning, "Invaild!");
		return;
	}

	auto pkt = PlayerDeadPacket(playerID);
	std::unordered_set<int32> vlist;
	{
		std::lock_guard lock(player->_vlLock);
		vlist = player->GetViewList();
	}
	SessionManager::GetInst().SendPacket(playerID, &pkt);
	SessionManager::GetInst().BroadcastToViewList(&pkt, vlist);
}

void GameWorld::CreateMonster()
{
	auto table = SpawnTable::GetInst();
	for (ZoneType z : { ZoneType::GYM, ZoneType::TUK, ZoneType::E, ZoneType::INDUSTY, ZoneType::BUNKER })
	{
		const auto& spawns = table.GetSpawnsByZone(z);
		ZoneType zone = (z == ZoneType::BUNKER) ? ZoneType::TUK : z;
		auto& zoneMap = _monstersByZone[zone];

		for (const auto& info : spawns)
		{
			for (int i = 0; i < info.Count; ++i)
			{
				int32 id = GenerateMonsterId();
				auto monster = std::make_shared<Monster>(id, zone, info.MonsterType);
				FVector pos;
				float radius = monster->GetInfo().CollisionRadius;
				if (!info.bRandomSpawn)
				{
					pos = info.SpawnPos;
				}
				else
				{
					do
					{
						pos = Map::GetInst().GetRandomPos(z, radius);
					} while (IsCollisionDetected(zone, pos, radius));
				}
				monster->SetPos(pos);
				monster->Init();
				monster->SetQuestID(static_cast<QuestType>(info.QuestID));
				if (info.DropItemID != -1)
				{
					monster->SetDropItem(info.DropItemID);
				}

				if (info.QuestID == -1)
					monster->SetActive(true);
				monster->SetBoss(info.bIsBoss);
				zoneMap[id] = monster;
			}
		}
		_monsterCnt[zone] = zoneMap.size();
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

		//zoneMap.erase(it);
		monster->SetActive(false);
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
				if (!monster || !monster->IsActive()) continue;
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

void GameWorld::UpdateMonsterState(int32 id, ECharacterStateType newState)
{
	auto monster = GetMonsterByID(id);
	if (!monster) return;
	monster->ChangeState(newState);
}

void GameWorld::HandleEarthQuakeImpact(const FVector& rockPos)
{
	const float damageRadius = 120.f;
	const float maxDamage = 50.f;

	std::lock_guard lock(_mtPlayerZMap);

	auto it = _playersByZone.find(ZoneType::GYM);
	if (it == _playersByZone.end()) return;

	for (auto& [id, player] : it->second)
	{
		if (!player || player->IsDead()) continue;

		const FVector& playerPos = player->GetInfo().Pos;
		float distSq = (playerPos - rockPos).LengthSquared();
		if (distSq <= damageRadius * damageRadius)
		{
			float dist = std::sqrt(distSq);
			float ratio = 1.0f - (dist / damageRadius);
			float damage = maxDamage * ratio;

			auto pkt = Tino::EarthQuakePacket(rockPos, true);
			SessionManager::GetInst().SendPacket(player->GetInfo().ID, &pkt);
			player->OnDamaged(damage);
		}
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

void GameWorld::SpawnGoldItem(FVector position)
{
	std::lock_guard<std::mutex> lock(_mtItem);
	auto newItem = std::make_shared<WorldItem>(position);

	ItemPkt::SpawnPacket packet(newItem->GetItemID(), newItem->GetItemTypeID(), position);
	_worldItems.emplace_back(newItem);
	SessionManager::GetInst().BroadcastToAll(&packet);
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
	if (!player) return FVector::ZeroVector;

	uint32 playerLevel = player->GetInfo().GetLevel();
	if (!Map::GetInst().IsZoneAccessible(targetZone, playerLevel))
	{
		LOG(std::format("Player [{}] cannot access due to level {}", playerId, playerLevel));
		return FVector::ZeroVector;
	}
	auto questData = player->GetCurrentQuestData();
	if (questData->Catagory == EQuestCategory::MOVE)
	{
		auto quest = questData->QuestID;
		switch (quest)
		{
		case QuestType::CH1_GO_TO_BUNKER:
			break;
		case QuestType::CH1_GO_TO_E_FIRST:
			break;
		case QuestType::CH2_ENTER_E_BUILDING:
			break;
		case QuestType::CH3_RETURN_TO_TIP_WITH_DOC:
			break;
		case QuestType::CH4_ENTER_GYM:
			if (targetZone == ZoneType::GYM)
			{
				CompleteQuest(playerId, quest);
			}
			break;
		}
	}
	ZoneType oldZone = player->GetZone();
	FVector newPos = Map::GetInst().GetRandomSpawnPosition(oldZone, targetZone);

	if (newPos == FVector::ZeroVector)
		return FVector::ZeroVector;

	player->SetPos(newPos);
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
			player->RemoveMonsterFromViewList(other);
		else
			player->RemovePlayerFromViewList(other);
	}

	{
		std::lock_guard lock(_mtPlayerZMap);
		auto& oldMap = _playersByZone[oldZone];
		oldMap.erase(playerId);
		if (oldMap.empty()) _playersByZone.erase(oldZone);
		_playersByZone[targetZone][playerId] = player;
	}

	UpdateViewList(player);
	ChangeZonePacket response(targetZone, newPos);
	SessionManager::GetInst().SendPacket(playerId, &response);
}

void GameWorld::RespawnPlayer(int32 playerId, ZoneType targetZone)
{
	auto player = GetPlayerByID(playerId);
	if (!player) return;
	ZoneType oldZone = player->GetZone();
	FVector newPos = Map::GetInst().GetRandomPos(targetZone, playerCollision);

	player->SetPos(newPos);
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
			player->RemoveMonsterFromViewList(other);
		else
			player->RemovePlayerFromViewList(other);
	}

	{
		std::lock_guard lock(_mtPlayerZMap);
		auto& oldMap = _playersByZone[oldZone];
		oldMap.erase(playerId);
		if (oldMap.empty()) _playersByZone.erase(oldZone);
		_playersByZone[targetZone][playerId] = player;
	}
	auto& info = player->GetInfo();
	info.Stats.Hp = info.Stats.MaxHp;
	info.State = ECharacterStateType::STATE_IDLE;

	RespawnPacket pkt(info);
	SessionManager::GetInst().SendPacket(playerId, &pkt);

	UpdateViewList(player);
	ChangeZonePacket response(targetZone, newPos);
	SessionManager::GetInst().SendPacket(playerId, &response);
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
				if (monster && monster->IsActive())
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

void GameWorld::RequestQuest(int32 playerId, QuestType quest)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invalid player");
		return;
	}

	player->SetCurrentQuest(quest);
}

void GameWorld::CompleteQuest(int32 playerId, QuestType quest)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invalid player");
		return;
	}
	if (player->IsQuestInProgress(quest))
		player->GiveQuestReward(quest);
}

void GameWorld::QuestSpawn(QuestType quest)
{
	std::lock_guard lock(_mtMonZMap);
	for (auto& [zone, monMap] : _monstersByZone)
	{
		for (auto& [id, mon] : monMap)
		{
			if (!mon) continue;
			if (!mon->IsActive() && mon->GetQuestID() == quest)
			{
				mon->SetActive(true);
			}
		}
	}
}

void GameWorld::BuyItem(int32 playerId, uint8 itemType, uint16 quantity)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invalid player");
		return;
	}

	bool bSuccess = false;
	DBResultCode  ResultCode = DBResultCode::SUCCESS;

	auto itemData = ItemTable::GetInst().GetItemByTypeId(itemType);
	if (!itemData)
	{
		LOG(Warning, "Invalid item data");
		ResultCode = DBResultCode::ITEM_NOT_FOUND;
		auto respkt = BuyItemResultPacket(bSuccess, ResultCode, 0);
		SessionManager::GetInst().SendPacket(playerId, &respkt);
		return;
	}
	uint32 price = itemData->Price * quantity;
	auto targetItem = std::make_shared<Item>(itemType);
	bSuccess = player->BuyItem(targetItem, price, quantity);

	if (bSuccess)
	{
		uint32 curgold = player->GetGold();
		auto pkt = ItemPkt::AddInventoryPacket(targetItem->GetItemID(), targetItem->GetItemTypeID());
		SessionManager::GetInst().SendPacket(playerId, &pkt);
		auto respkt = BuyItemResultPacket(bSuccess, ResultCode, curgold);
		SessionManager::GetInst().SendPacket(playerId, &respkt);
	}
	else
	{
		ResultCode = DBResultCode::NOT_ENOUGH_GOLD;
		auto respkt = BuyItemResultPacket(bSuccess, ResultCode, 0);
		SessionManager::GetInst().SendPacket(playerId, &respkt);
	}
}

void GameWorld::SellItem(int32 playerId, uint32 itemId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invalid player");
		return;
	}
	player->SellItem(itemId);
}

