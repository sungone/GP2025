#include "pch.h"
#include "GameManager.h"

void GameManager::Init()
{
	CreateMonster();
	StartMonsterStateBroadcast();
	StartMonsterAIUpdate();
}

void GameManager::AddPlayer(int32 id, std::shared_ptr<Character> player)
{
	_characters[id] = player;
	_characters[id]->GetInfo().ID = id;
}

void GameManager::RemoveCharacter(int32 id)
{
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
	if (damaged.ID < MAX_PLAYER || damaged.ID >= MAX_CHARACTER || !_characters[damaged.ID] || !_characters[damaged.ID]->IsValid())
	{
		LOG(Warning, "Invalid");
		return false;
	}

	auto& charater = _characters[damaged.ID];
	charater->OnDamaged(damage);
	return true;
}

std::shared_ptr<Character> GameManager::GetCharacterByID(int32 id)
{
	if (id < 0 || id >= MAX_CHARACTER || !_characters[id] || !_characters[id]->IsValid())
	{
		LOG(Warning, "Invalid");
		return nullptr;
	}
	return _characters[id];
}