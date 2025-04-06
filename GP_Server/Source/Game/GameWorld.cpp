#include "pch.h"
#include "GameWorld.h"


bool GameWorld::Init()
{
	if (!ItemTable::GetInst().LoadFromCSV("../DataTable/ItemTable.csv"))
	{
		LOG(LogType::Warning, "LoadFromCSV");
		return false;
	}

	if (!PlayerLevelTable::GetInst().LoadFromCSV("../DataTable/PlayerLevelTable.csv"))
	{
		LOG(LogType::Warning, "LoadFromCSV");
		return false;
	}

	if (!PlayerSkillTable::GetInst().LoadFromCSV("../DataTable/PlayerSkillTable.csv"))
	{
		LOG(LogType::Warning, "LoadFromCSV");
		return false;
	}

	if (!MonsterTable::GetInst().LoadFromCSV("../DataTable/MonsterTable.csv"))
	{
		LOG(LogType::Warning, "LoadFromCSV");
		return false;
	}

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
		SessionManager::GetInst().BroadcastToViewList(&Pkt, id);
	}
	else
	{
		LOG(Log, std::format("Remove Monster"));
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_MONSTER, _characters[id]->GetInfo());
		SessionManager::GetInst().BroadcastToViewList(&Pkt, id);
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
	TimerQueue::AddTimerEvent(TimerEvent(0, ::MONSTER_UPDATE, 2000));
}

void GameWorld::SpawnMonster(PlayerSession& session)
{
	int32 playerId = session.GetId();
	std::shared_ptr<Character> playerCharacter = GetCharacterByID(playerId);

	if (!playerCharacter) return;
	const auto& viewList = playerCharacter->GetViewList();

	for (int32 i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (_characters[i]) continue;
		auto& monster = _characters[i];
		int32 monsterId = monster->GetInfo().ID;
		if (viewList.find(monsterId) != viewList.end())
		{
			auto Pkt = InfoPacket(EPacketType::S_ADD_MONSTER, monster->GetInfo());
			session.DoSend(&Pkt);
		}
	}
}


void GameWorld::PlayerMove(int32 playerId, FVector& pos, uint64& time)
{
	auto player = GetCharacterByID(playerId);
	if (!player)
	{
		LOG(Warning, "Invaild");
		return;
	}
	player->GetInfo().SetLocationAndYaw(pos);
	auto pkt = MovePacket(playerId, time);
	SessionManager::GetInst().SendPacket(playerId, &pkt);
	SessionManager::GetInst().BroadcastToViewList(&pkt, playerId);
}

void GameWorld::PlayerAttack(int32 playerId, float playerYaw)
{
	auto player = std::dynamic_pointer_cast<Player>(_characters[playerId]);
	if (!player)
	{
		LOG(Warning, "Invaild!");
		return;
	}
	player->ChangeState(ECharacterStateType::STATE_AUTOATTACK);
	player->GetInfo().SetYaw(playerYaw);

	for (auto& monsterId : player->GetViewList())
	{
		if (monsterId < MAX_PLAYER) continue;

		auto monster = _characters[monsterId];
		if (!monster) continue;
		if (player->IsInAttackRange(monster->GetInfo()))
		{
			float atkDamage = player->GetAttackDamage();
			if (atkDamage > 0.0f)
			{
				monster->OnDamaged(atkDamage);
			}

			auto pkt = MonsterDamagePacket(monster->GetInfo(), atkDamage);
			SessionManager::GetInst().SendPacket(playerId, &pkt);
			SessionManager::GetInst().BroadcastToViewList(&pkt, playerId);

			if (monster->IsDead())
			{
				player->AddExp(10);
				SpawnWorldItem({ monster->GetInfo().Pos.X, monster->GetInfo().Pos.Y, monster->GetInfo().Pos.Z + 20 });
				RemoveCharacter(monsterId);
			}
		}
	}

	auto infopkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, player->GetInfo());
	SessionManager::GetInst().SendPacket(playerId, &infopkt);
	SessionManager::GetInst().BroadcastToViewList(&infopkt, playerId);
}

void GameWorld::UpdateMonster()
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (!_characters[i]) continue;

		_characters[i]->Update();
	}
	BroadcastMonsterStates();
}

void GameWorld::BroadcastMonsterStates()
{
	for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (!_characters[i]) continue;
		auto& monster = _characters[i];
		FInfoData MonsterInfoData = monster->GetInfo();
		InfoPacket packet(S_MONSTER_STATUS_UPDATE, MonsterInfoData);
		const auto& viewList = monster->GetViewList();
		SessionManager::GetInst().BroadcastToViewList(&packet, i);
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
	ItemPkt::SpawnPacket packet(newItem->GetItemID(), newItem->GetItemType(), position);
	_worldItems.emplace_back(newItem);
	SessionManager::GetInst().BroadcastToAll(&packet);
}

void GameWorld::SpawnWorldItem(WorldItem dropedItem)
{
	std::lock_guard<std::mutex> lock(_iMutex);
	auto newItem = std::make_shared<WorldItem>(dropedItem);
	_worldItems.emplace_back(newItem);
	ItemPkt::DropPacket packet(newItem->GetItemID(), newItem->GetItemType(), newItem->GetPos());
	SessionManager::GetInst().BroadcastToAll(&packet);
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
		SessionManager::GetInst().BroadcastToAll(&pkt1);
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
	SessionManager::GetInst().BroadcastToViewList(&pkt1, playerId);
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
	SessionManager::GetInst().BroadcastToViewList(&pkt1, playerId);
}

void GameWorld::UpdateViewList(std::shared_ptr<Character> listOwner)
{
	int32 ownerId = listOwner->GetInfo().ID;

	for (int32 otherId = 1; otherId < MAX_CHARACTER; ++otherId)
	{
		auto other = _characters[otherId];
		if (!other)continue;
		if (otherId == ownerId) continue;
		listOwner->UpdateViewList(other);
	}
}

bool GameWorld::IsCollisionDetected(const FVector& pos)
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	for (auto other : _characters)
	{
		if (!other) continue;
		if (other->IsColision(pos)) return true;
	}
	return false;
}
