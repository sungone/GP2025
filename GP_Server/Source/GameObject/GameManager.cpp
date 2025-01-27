#include "GameManager.h"

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
		
	}
}