#include "GameManager.h"

void GameManager::Init()
{
	CreateMonster();
	StartMonsterStateBroadcast();
	StartMonsterAIUpdate();
}

void GameManager::AddPlayer(int id, std::shared_ptr<Character> player)
{
	characters[id] = player;
	characters[id]->GetInfo().ID = id;
}

void GameManager::RemoveCharacter(int id)
{
	if (id < 0 || id >= MAX_CHARACTER || !characters[id])
	{
		LOG(Warning, "Invalid");
		return;
	}
	characters[id] = nullptr;
}

void GameManager::CreateMonster()
{
	for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		characters[i] = std::make_shared<Monster>();
		characters[i]->Init();
		characters[i]->GetInfo().ID = i;
	}
}

void GameManager::SpawnMonster(Session& session)
{
	for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (characters[i] && characters[i]->IsValid())
		{
			auto Pkt = InfoPacket(EPacketType::S_ADD_MONSTER, characters[i]->GetInfo());
			session.DoSend(&Pkt);
		}
	}
}

void GameManager::OnDamaged(float damage, FInfoData& damaged)
{
	if (damaged.ID < MAX_PLAYER || damaged.ID >= MAX_CHARACTER || !characters[damaged.ID] || !characters[damaged.ID]->IsValid())
	{
		LOG(Warning, "Invalid");
		return;
	}

	auto& charater = characters[damaged.ID];
	charater->OnDamaged(damage);
}

std::shared_ptr<Character> GameManager::GetCharacterByID(int id)
{
	if (id < 0 || id >= MAX_CHARACTER || !characters[id] || !characters[id]->IsValid())
	{
		LOG(Warning, "Invalid");
		return nullptr;
	}
	return characters[id];
}