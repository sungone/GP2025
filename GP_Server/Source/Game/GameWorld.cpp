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
		LOG_E("LoadFromCSV");
		return false;
	}

	CreateMonster();
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
	ZoneType startZone = START_ZONE;
	newPos = Map::GetInst().GetStartPos(startZone);
	newPos.Z += 90.f;

	player->UpdatePos(newPos);
	int32 playerId = player->GetInfo().ID;
	player->GetInfo().SetZone(startZone);

	{
		std::lock_guard lock(_mtPlayers);
		_players[playerId] = player;
	}

	{
		std::lock_guard lock(_mtPlayerZMap);
		_playersByZone[startZone][playerId] = player;
	}

	//for test
	player->AddGold(10000);
	BuyItem(playerId, (uint8)Type::EQuestItem::KEY, 1);

	auto& playerInfo = player->GetInfo();

	EnterGamePacket enterpkt(playerInfo);
	SessionManager::GetInst().SendPacket(playerId, &enterpkt);
	player->OnEnterGame();
	InitViewList(playerId, startZone);
	AddItems(playerId, startZone);

	LOG_D("Enter Game [{}] - Zone <{}>", playerId, ENUM_NAME(startZone));
	auto questData = player->GetCurrentQuestData();
	if (questData->Catagory == EQuestCategory::KILL)
		QuestSpawn(playerId, questData->QuestID);
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
		LOG_W("Invaild!");
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
		LOG_W("Invaild!");
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
		LOG_W("Invaild!");
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
		LOG_W("Invaild!");

		return;
	}
	LOG_D("Player [{}] Move {}", playerId, pos.ToString());
	player->UpdatePos(pos);
	player->GetInfo().State = static_cast<ECharacterStateType>(state);
	UpdateViewList(playerId);

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
		LOG_W("Invaild!");
		return;
	}

	std::unordered_set<int32> viewList;
	{
		std::lock_guard lock(player->_vlLock);
		viewList = player->GetViewList();
	}
	auto zone = player->GetZone();
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
			uint32 dropId = monster->GetDropItemId();
			if (monster->IsBoss())
			{
				if (monster->HasDropItem())
				{
					FVector itemPos = basePos + RandomUtils::GetRandomOffset();
					auto dropedItem = WorldItem(dropId, itemPos);
					SpawnWorldItem(dropedItem, zone);
				}
				player->CheckAndUpdateQuestProgress();
			}
			if (mquest != QuestType::NONE)
			{
				if (mquest == QuestType::CH2_CLEAR_E_BUILDING || mquest == QuestType::CH3_CLEAR_SERVER_ROOM)
				{
					//todo: 
					// 다른플레이어가 죽은 몬스터 활성화 못하도록
					// 몬스터에 퀘스트 진행중 체크 넣야할듯
					_monsterCnt[zone]--;
				}
				player->CheckAndUpdateQuestProgress();
			}
			//todo: 아이템 드랍테이블로 스폰하자
			{
				FVector itemPos = basePos + RandomUtils::GetRandomOffset();
				SpawnWorldItem(itemPos, monlv, playertype, zone);
				FVector goldPos = basePos + RandomUtils::GetRandomOffset();
				SpawnGoldItem(goldPos, zone);
			}

			OnMonsterDead(targetId);
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
		LOG_W("Invaild!");
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
		LOG_W("Invaild!");
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
	for (ZoneType z : { ZoneType::TUK, ZoneType::E, ZoneType::INDUSTY, ZoneType::BUNKER, ZoneType::GYM })
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
				do
				{
					pos = Map::GetInst().GetRandomPos(z);
				} while (IsCollisionDetected(zone, pos, radius));
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
				if (zone == ZoneType::TUK)
					GameWorld::GetInst().EnterGrid(id, pos);
			}
		}
		_monsterCnt[zone] = zoneMap.size();
	}
}

void GameWorld::OnMonsterDead(int32 monsterId)
{
	auto monster = GetMonsterByID(monsterId);
	auto pkt = MonsterDeadPacket(monsterId);
	std::unordered_set<int32> viewList;
	{
		std::lock_guard lock(monster->_vlLock);
		viewList = monster->GetViewList();
	}
	SessionManager::GetInst().BroadcastToViewList(&pkt, viewList);

	for (auto& pid : viewList)
	{
		auto player = GetPlayerByID(pid);
		if (!player) continue;
		player->RemoveFromViewList(monsterId);
	}

	monster->SetActive(false);
	if (monster->GetQuestID() == QuestType::NONE)
	{
		TimerQueue::AddTimer([monsterId]() {
			GameWorld::GetInst().MonsterRespawn(monsterId);
			}, MONSTER_RESPAWN_TIME_MS, false);
	}
}

void GameWorld::UpdateMonsterState(int32 id, ECharacterStateType newState)
{
	auto monster = GetMonsterByID(id);
	if (!monster) return;
	monster->ChangeState(newState);
}

void GameWorld::MonsterRespawn(int32 monsterId)
{
	auto monster = GetMonsterByID(monsterId);
	if (!monster)
	{
		LOG_W("Invalid monster");
		return;
	}
	monster->Respawn();
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


bool GameWorld::RemoveWorldItem(uint32 itemId, ZoneType zone)
{
	auto& items = _worldItemsByZone[zone];
	auto it = std::remove_if(items.begin(), items.end(),
		[itemId](const std::shared_ptr<WorldItem>& item) {
			return item->GetItemID() == itemId;
		});
	if (it != items.end())
	{
		items.erase(it, items.end());
		return true;
	}
	return false;
}

std::shared_ptr<WorldItem> GameWorld::FindWorldItemById(uint32 itemId, ZoneType zone)
{
	auto zoneIt = _worldItemsByZone.find(zone);
	if (zoneIt == _worldItemsByZone.end()) return nullptr;

	auto& itemList = zoneIt->second;
	auto it = std::find_if(itemList.begin(), itemList.end(),
		[itemId](const std::shared_ptr<WorldItem>& item) {
			return item->GetItemID() == itemId;
		});

	if (it != itemList.end())
		return *it;

	return nullptr;
}

void GameWorld::SpawnGoldItem(FVector position, ZoneType zone)
{
	std::lock_guard<std::mutex> lock(_mtItemZMap);
	auto newItem = std::make_shared<WorldItem>(position);
	_worldItemsByZone[zone].emplace_back(newItem);

	auto itemId = newItem->GetItemID();
	ItemPkt::SpawnPacket packet(itemId, newItem->GetItemTypeID(), position);
	BroadcastToZone(zone, &packet);

	TimerQueue::AddTimer([itemId, zone]() {
		GameWorld::GetInst().DespawnWorldItem(itemId, zone);
		}, ITEM_DISAPPEAR_TIME_MS, false);
}

void GameWorld::SpawnWorldItem(FVector position, uint32 monlv, Type::EPlayer playertype, ZoneType zone)
{
	std::lock_guard<std::mutex> lock(_mtItemZMap);
	auto newItem = std::make_shared<WorldItem>(position, monlv, playertype);
	_worldItemsByZone[zone].emplace_back(newItem);

	auto itemId = newItem->GetItemID();
	ItemPkt::SpawnPacket packet(itemId, newItem->GetItemTypeID(), position);
	BroadcastToZone(zone, &packet);

	TimerQueue::AddTimer([itemId, zone]() {
		GameWorld::GetInst().DespawnWorldItem(itemId, zone);
		}, ITEM_DISAPPEAR_TIME_MS, false);
}

void GameWorld::SpawnWorldItem(WorldItem dropedItem, ZoneType zone)
{
	std::lock_guard<std::mutex> lock(_mtItemZMap);
	auto newItem = std::make_shared<WorldItem>(dropedItem);
	_worldItemsByZone[zone].emplace_back(newItem);

	int32 itemId = newItem->GetItemID();
	ItemPkt::DropPacket packet(itemId, newItem->GetItemTypeID(), newItem->GetPos());
	BroadcastToZone(zone, &packet);
	if (newItem->GetItemCategory() != EItemCategory::Quest)
	{
		TimerQueue::AddTimer([itemId, zone]() {
			GameWorld::GetInst().DespawnWorldItem(itemId, zone);
			}, ITEM_DISAPPEAR_TIME_MS, false);
	}
}

void GameWorld::DespawnWorldItem(uint32 itemId, ZoneType zone)
{
	std::lock_guard<std::mutex> lock(_mtItemZMap);

	auto& itemList = _worldItemsByZone[zone];
	auto item = std::find_if(itemList.begin(), itemList.end(),
		[itemId](const std::shared_ptr<WorldItem>& item) {
			return item->GetItemID() == itemId;
		});

	if (item == itemList.end())
		return;

	auto pos = (*item)->GetPos();
	itemList.erase(item);

	auto pkt = ItemPkt::DespawnPacket(itemId);
	BroadcastToZone(zone, &pkt);

	LOG_D("Item [{}] auto-despawned", itemId);
}

void GameWorld::ClearItems(int32 playerId, ZoneType oldZone)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
		return;
	}
	std::lock_guard<std::mutex> lock(_mtItemZMap);
	{
		auto& itemList = _worldItemsByZone[oldZone];
		for (auto item : itemList)
		{
			auto i = item->GetItemID();
			auto pkt = ItemPkt::DespawnPacket(i);
			SessionManager::GetInst().SendPacket(playerId, &pkt);
		}
	}
}

void GameWorld::AddItems(int32 playerId, ZoneType newZone)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
		return;
	}
	std::lock_guard<std::mutex> lock(_mtItemZMap);
	{
		auto& itemList = _worldItemsByZone[newZone];
		for (auto item : itemList)
		{
			auto i = item->GetItemID();
			auto type = item->GetItemTypeID();
			auto pos = item->GetPos();
			auto pkt = ItemPkt::SpawnPacket(i, type, pos);
			SessionManager::GetInst().SendPacket(playerId, &pkt);
		}
	}
}

void GameWorld::PickUpWorldItem(int32 playerId, uint32 itemId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid player in PickUpWorldItem");
		return;
	}
	std::lock_guard<std::mutex> lock(_mtItemZMap);
	auto zone = player->GetZone();
	auto targetItem = FindWorldItemById(itemId, zone);
	if (!targetItem)
	{
		LOG_W("Invalid");
		return;
	}

	if (player->TakeWorldItem(targetItem))
	{
		RemoveWorldItem(itemId, zone);
		auto pkt = ItemPkt::AddInventoryPacket(targetItem->GetItemID(), targetItem->GetItemTypeID());
		SessionManager::GetInst().SendPacket(playerId, &pkt);
		auto pkt1 = ItemPkt::PickUpPacket(itemId);
		BroadcastToZone(zone, &pkt1);
	}
	else
	{
		LOG_D("Failed TakeItem");
	}
}

void GameWorld::UseInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
		return;
	}
	player->UseItem(itemId);
}

void GameWorld::EquipInventoryItem(int32 playerId, uint32 itemId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
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
		LOG_W("Invalid");
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

bool GameWorld::TransferToZone(int32 playerId, ZoneType newZone)
{
	LOG_D("Try Transfer");
	auto player = GetPlayerByID(playerId);
	if (!player) return false;
	auto& info = player->GetInfo();
	uint32 playerLevel = info.GetLevel();
	if (!Map::GetInst().IsZoneAccessible(newZone, playerLevel))
	{
		LOG_D("Player [{}] cannot access due to level {}", playerId, playerLevel);
		return false;
	}
	ZoneType oldZone = player->GetZone();
	auto oldName = ENUM_NAME(oldZone);
	auto newName = ENUM_NAME(newZone);
	LOG_D("Transfer <{}> To <{}>", oldName, newName);

	if (oldZone == ZoneType::TUK)
		LeaveGrid(playerId, player->GetPos());

	FVector newPos = Map::GetInst().GetRandomEntryPos(oldZone, newZone);
	if (newPos == FVector::ZeroVector)
	{
		LOG_W("EntryPos");
		return false;
	}
	newPos.Z += 90.f;
	LOG_D("({}, {}, {})", newPos.X, newPos.Y, newPos.Z);

	info.SetZone(newZone);
	player->UpdatePos(newPos);

	ClearViewList(playerId);
	ClearItems(playerId, oldZone);

	{
		std::lock_guard lock(_mtPlayerZMap);
		auto& oldMap = _playersByZone[oldZone];
		oldMap.erase(playerId);
		if (oldMap.empty()) _playersByZone.erase(oldZone);
		_playersByZone[newZone][playerId] = player;
	}

	ChangeZonePacket response(newZone, newPos);
	SessionManager::GetInst().SendPacket(playerId, &response);

	InitViewList(playerId, newZone);
	AddItems(playerId, newZone);
	return true;
}

void GameWorld::RespawnPlayer(int32 playerId, ZoneType newZone)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid player");
		return;
	}
	ZoneType oldZone = player->GetZone();
	if (oldZone == ZoneType::TUK)
		LeaveGrid(playerId, player->GetPos());
	FVector newPos = Map::GetInst().GetRandomPos(newZone);
	newPos.Z += 90.f;
	auto& info = player->GetInfo();
	info.SetZone(newZone);
	player->UpdatePos(newPos);
	auto oldName = ENUM_NAME(oldZone);
	auto newName = ENUM_NAME(newZone);
	LOG_D("Respawn <{}> To <{}>", oldName, newName);
	player->Restore();

	ClearViewList(playerId);
	ClearItems(playerId, oldZone);
	{
		std::lock_guard lock(_mtPlayerZMap);
		auto& oldMap = _playersByZone[oldZone];
		oldMap.erase(playerId);
		if (oldMap.empty()) _playersByZone.erase(oldZone);
		_playersByZone[newZone][playerId] = player;
	}

	RespawnPacket pkt(info);
	SessionManager::GetInst().SendPacket(playerId, &pkt);

	InitViewList(playerId, newZone);
	AddItems(playerId, newZone);
}

void GameWorld::UpdateViewList(int32 playerId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
		return;
	}
	ZoneType ownerZone = player->GetZone();
	int32 ownerId = player->GetInfo().ID;
	if (IsMonster(ownerId)) return;

	if (ownerZone != ZoneType::TUK)
		return;

	auto idList = QueryNearbyCharacters(player->GetPos());
	for (int32 id : idList)
	{
		if (id == ownerId) continue;
		auto target = GetCharacterByID(id);
		if (!target || !target->IsValid()) continue;
		player->UpdateViewList(target);
	}
}

void GameWorld::ClearViewList(int32 playerId)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
		return;
	}
	std::unordered_set<int32> oldvlist;
	{
		std::lock_guard lock(player->_vlLock);
		oldvlist = player->GetViewList();
	}
	for (int32 mid : oldvlist)
	{
		auto other = GetCharacterByID(mid);
		if (!other || !other->IsValid()) continue;
		if (other->IsMonster())
			player->RemoveMonsterFromViewList(other);
		else
			player->RemovePlayerFromViewList(other);
	}
}

void GameWorld::InitViewList(int32 playerId, ZoneType zone)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
		return;
	}

	int32 ownerId = player->GetInfo().ID;
	if (IsMonster(ownerId)) return;

	if (zone != ZoneType::TUK)
	{
		AddAllToViewList(player, zone);
		return;
	}
	auto idList = QueryNearbyCharacters(player->GetPos());
	for (int32 id : idList)
	{
		if (id == ownerId) continue;
		auto target = GetCharacterByID(id);
		if (!target || !target->IsValid()) continue;
		player->UpdateViewList(target);
	}
}

void GameWorld::AddAllToViewList(std::shared_ptr<Player> player, ZoneType zone)
{
	int32 ownerId = player->GetInfo().ID;
	{
		std::lock_guard lock(_mtPlayerZMap);
		for (auto& [pid, otherPlayer] : _playersByZone[zone])
		{
			if (otherPlayer && pid != ownerId)
			{
				player->AddPlayerToViewList(otherPlayer);
			}
		}
	}

	{
		std::lock_guard lock(_mtMonZMap);
		for (auto& [mid, monster] : _monstersByZone[zone])
		{
			if (monster && monster->IsActive())
			{
				player->AddMonsterToViewList(monster);
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
		LOG_W("Invalid");
		return;
	}

	player->SetCurrentQuest(quest);
}

void GameWorld::CompleteQuest(int32 playerId, QuestType quest)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
		return;
	}
	if(player->IsQuestInProgress(quest))
		player->CheckAndUpdateQuestProgress();
}

void GameWorld::QuestSpawn(int32 playerId, QuestType quest)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
		return;
	}
	{
		std::lock_guard lock(_mtMonZMap);
		for (auto& [zone, monMap] : _monstersByZone)
		{
			for (auto& [id, mon] : monMap)
			{
				if (!mon) continue;
				if (!mon->IsActive() && mon->GetQuestID() == quest)
				{
					mon->Respawn();
				}
			}
		}
	}

	AddAllToViewList(player, player->GetZone());
}

void GameWorld::BuyItem(int32 playerId, uint8 itemType, uint16 quantity)
{
	auto player = GetPlayerByID(playerId);
	if (!player)
	{
		LOG_W("Invalid");
		return;
	}

	bool bSuccess = false;
	DBResultCode  ResultCode = DBResultCode::SUCCESS;

	auto itemData = ItemTable::GetInst().GetItemByTypeId(itemType);
	if (!itemData)
	{
		LOG_W("Invalid");
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
		LOG_W("Invalid");
		return;
	}
	player->SellItem(itemId);
}

void GameWorld::BroadcastToZone(ZoneType zone, Packet* packet)
{
	std::lock_guard<std::mutex> lock(_mtPlayerZMap);

	auto it = _playersByZone.find(zone);
	if (it == _playersByZone.end())
		return;

	for (auto& [playerId, player] : it->second)
	{
		if (player)
		{
			SessionManager::GetInst().SendPacket(playerId, packet);
		}
	}
}

GridPos GameWorld::GetGridPos(const FVector& pos)
{
	int32 x = static_cast<int32>(std::floor(pos.X / GRID_CELL_SIZE));
	int32 y = static_cast<int32>(std::floor(pos.Y / GRID_CELL_SIZE));
	return GridPos{ x, y };
}

void GameWorld::EnterGrid(int32 id, const FVector& pos)
{
	std::lock_guard<std::mutex> lock(_gridMutex);
	_gridMap[GetGridPos(pos)].insert(id);
}

void GameWorld::LeaveGrid(int32 id, const FVector& pos)
{
	std::lock_guard<std::mutex> lock(_gridMutex);
	GridPos gp = GetGridPos(pos);
	auto it = _gridMap.find(gp);
	if (it != _gridMap.end())
	{
		it->second.erase(id);
		if (it->second.empty())
			_gridMap.erase(gp);
	}
}

void GameWorld::MoveGrid(int32 id, const FVector& oldPos, const FVector& newPos)
{
	GridPos oldGrid = GetGridPos(oldPos);
	GridPos newGrid = GetGridPos(newPos);

	if (oldGrid.X != newGrid.X || oldGrid.Y != newGrid.Y)
	{
		LeaveGrid(id, oldPos);
		EnterGrid(id, newPos);
	}
}

std::vector<int32> GameWorld::QueryNearbyCharacters(const FVector& pos)
{
	std::vector<int32> result;
	GridPos center = GetGridPos(pos);
	int32 range = static_cast<int32>(std::ceil(VIEW_DIST / GRID_CELL_SIZE));

	std::lock_guard<std::mutex> lock(_gridMutex);

	for (int32 dx = -range; dx <= range; ++dx)
	{
		for (int32 dy = -range; dy <= range; ++dy)
		{
			GridPos neighbor{ center.X + dx, center.Y + dy };
			auto it = _gridMap.find(neighbor);
			if (it != _gridMap.end())
			{
				result.insert(result.end(), it->second.begin(), it->second.end());
			}
		}
	}
	return result;
}