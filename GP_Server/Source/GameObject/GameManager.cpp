#include "GameManager.h"

void GameManager::Init()
{
	CreateMonster();
	StartMonsterStateBroadcast();
	StartMonsterAIUpdate();
}

void GameManager::AddPlayer(int32 id, std::shared_ptr<Character> player)
{
	characters[id] = player;
	characters[id]->GetInfo().ID = id;
}

void GameManager::RemoveCharacter(int32 id)
{
	if (id < 0 || id >= MAX_CHARACTER || !characters[id])
	{
		LOG(Warning, "Invalid");
		return;
	}

	if (id < MAX_PLAYER)
	{
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_PLAYER, characters[id]->GetInfo());
		SessionManager::GetInst().Broadcast(&Pkt);
	}
	else
	{
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_MONSTER, characters[id]->GetInfo());
		SessionManager::GetInst().Broadcast(&Pkt);
	}

	characters[id] = nullptr;
}

void GameManager::CreateMonster()
{
	for (int32 i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		characters[i] = std::make_shared<Monster>();
		characters[i]->Init();
		characters[i]->GetInfo().ID = i;
	}
}

void GameManager::SpawnMonster(Session& session)
{
	for (int32 i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (characters[i] && characters[i]->IsValid())
		{
			auto Pkt = InfoPacket(EPacketType::S_ADD_MONSTER, characters[i]->GetInfo());
			session.DoSend(&Pkt);
		}
	}
}

bool GameManager::OnDamaged(float damage, FInfoData& damaged)
{
	if (damaged.ID < MAX_PLAYER || damaged.ID >= MAX_CHARACTER || !characters[damaged.ID] || !characters[damaged.ID]->IsValid())
	{
		LOG(Warning, "Invalid");
		return false;
	}

	auto& charater = characters[damaged.ID];
	charater->OnDamaged(damage);
	return true;
}

std::shared_ptr<Character> GameManager::GetCharacterByID(int32 id)
{
	if (id < 0 || id >= MAX_CHARACTER || !characters[id] || !characters[id]->IsValid())
	{
		LOG(Warning, "Invalid");
		return nullptr;
	}
	return characters[id];
}