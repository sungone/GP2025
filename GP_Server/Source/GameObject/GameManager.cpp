#include "pch.h"
#include "GameManager.h"

bool GameManager::Init()
{
	CreateMonster();
	StartMonsterStateBroadcast();
	StartMonsterAIUpdate();
	return true;
}

void GameManager::AddPlayer(int32 id, std::shared_ptr<Character> player)
{
	std::unique_lock<std::mutex> lock(_carrMutex);

	_characters[id] = player;
	_characters[id]->GetInfo().ID = id;
}

void GameManager::RemoveCharacter(int32 id)
{
	std::unique_lock<std::mutex> lock(_carrMutex);

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

bool GameManager::OnDamaged(float damage, FInfoData& damaged)
{
	std::unique_lock<std::mutex> lock(_carrMutex);

	if (damaged.ID < MAX_PLAYER || damaged.ID >= MAX_CHARACTER || !_characters[damaged.ID] || !_characters[damaged.ID]->IsValid())
	{
		LOG(Warning, "Invalid");
		return false;
	}

	auto& character = _characters[damaged.ID];
	character->OnDamaged(damage);
	return true;
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

	for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (_characters[i] && _characters[i]->IsValid())
		{
			auto& monster = _characters[i];
			if (monster->IsDead())
			{
				LOG(SendLog, std::format("Remove monster[{}]", monster->GetInfo().ID));
				monster->GetInfo().State = ECharacterStateType::STATE_DIE;
				RemoveCharacter(i);
				continue;
			}
			FInfoData MonsterInfoData = monster->GetInfo();
			MonsterInfoPacket packet(S_MONSTER_STATUS_UPDATE, MonsterInfoData);
			SessionManager::GetInst().Broadcast(&packet);
			LOG(SendLog, std::format("Update monster[{}]", monster->GetInfo().ID));
		}
	}
}

void GameManager::StartMonsterAIUpdate()
{
	_MonsterAIUpdateTimer.Start(4000, [this]() {
		std::unique_lock<std::mutex> lock(_carrMutex);
		for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
		{
			if (_characters[i] && _characters[i]->IsValid())
			{
				_characters[i]->Update();
			}
		}
		});
}