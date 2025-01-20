#include "MonsterManager.h"
#include "iostream"

void MonsterManager::SpawnMonster(int32 ID, ECharacterType MonsterType , float X , float Y , float Z ,float Yaw)
{
    if (Monsters.find(ID) != Monsters.end())
    {
        std::cerr << "Monster with ID " << ID << " already exists!\n";
        return;
    }

    Monsters.emplace(ID, Monster(ID, MonsterType , X , Y , Z , Yaw));
    std::cout << "Spawned Monster: ID=" << ID << ", MonsterType = " << MonsterType
        << ", Location=(" << X << ", " << Y << ", " << Z << ")\n";
}

void MonsterManager::RemoveMonster(int32 ID)
{
    auto It = Monsters.find(ID);
    if (It != Monsters.end())
    {
        Monsters.erase(It);
        std::cout << "Removed Monster with ID: " << ID << "\n";
    }
    else
    {
        std::cerr << "Monster with ID " << ID << " not found!\n";
    }
}

Monster* MonsterManager::GetMonsterByID(int32 ID)
{
    auto It = Monsters.find(ID);
    if (It != Monsters.end())
    {
        return &It->second;
    }

	return nullptr;
}

const std::unordered_map<int32, Monster>& MonsterManager::GetAllMonsters() const
{
    return Monsters;
}
