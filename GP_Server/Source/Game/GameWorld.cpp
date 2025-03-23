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
		SessionManager::GetInst().BroadcastToViewList(&Pkt, id);
	}
	else
	{
		LOG(Log, std::format("Remove Monster"));
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_MONSTER, _characters[id]->GetInfo());
		SessionManager::GetInst().BroadcastToViewList(&Pkt, id);
	}

	std::unique_lock<std::mutex> lock(_carrMutex);
	_characters[id] = nullptr;
}

void GameWorld::CreateMonster()
{
	LOG("CreateMonster!");
	for (int32 i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		_characters[i] = std::make_shared<Monster>();
		_characters[i]->Init();
		_characters[i]->GetInfo().ID = i;
	}
	TimerQueue::AddTimerEvent(TimerEvent(0, ::MONSTER_UPDATE, 2000));
}

void GameWorld::SpawnMonster(Session& session)
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


void GameWorld::PlayerMove(int32 playerId, FInfoData& info)
{
	auto player = _characters[playerId];
	player->SetInfo(info);
	LOG(std::format("Player[{}] Move to {}", playerId, info.Pos.ToString()));
	auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, info);
	SessionManager::GetInst().BroadcastToViewList(&pkt, playerId);
}

void GameWorld::PlayerAttack(int32 playerId, int32 monsterId)
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	auto& Attacker = _characters[playerId];
	auto& atkInfo = Attacker->GetInfo();
	atkInfo.AddState(ECharacterStateType::STATE_AUTOATTACK);
	if (monsterId != -1)
	{

		LOG(Log, std::format("Attacked monster[{}]", monsterId));
		std::shared_ptr<Monster> Target = static_pointer_cast<Monster>(_characters[monsterId]);
		if (Attacker->IsInAttackRange(Target->GetInfo()))
		{
#ifdef _DEBUG
			float atkDamage = 50;
#else
			float atkDamage = Attacker->GetAttackDamage();
#endif // TEST
			if (atkDamage > 0.0f)
			{
				Target->OnDamaged(atkDamage);
			}

			auto pkt = DamagePacket(Target->GetInfo(), atkDamage);
			SessionManager::GetInst().SendPacket(playerId, &pkt);
			SessionManager::GetInst().BroadcastToViewList(&pkt, playerId);
			if (Target->IsDead())
			{
				atkInfo.AddExp(10);
				SpawnWorldItem({ Target->GetInfo().Pos.X,Target->GetInfo().Pos.Y,Target->GetInfo().Pos.Z + 120 });
				RemoveCharacter(monsterId);
			}
		}
	}
	auto infopkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, atkInfo);
	SessionManager::GetInst().SendPacket(playerId, &infopkt);
	SessionManager::GetInst().BroadcastToViewList(&infopkt, playerId);
}

void GameWorld::UpdateMonster()
{
	LOG("Update Monster!");
	std::unique_lock<std::mutex> lock(_carrMutex);
	for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (!_characters[i]) return;

		UpdateViewList(_characters[i]);
		_characters[i]->Update();
	}
	BroadcastMonsterStates();
}

void GameWorld::BroadcastMonsterStates()
{
	LOG("BroadcastToViewList Monster States!");
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
