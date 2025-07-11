#include "pch.h"
#include "MonsterTable.h"

bool MonsterTable::LoadFromCSV(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        return false;
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;

        FMonsterData monster;

        std::getline(ss, cell, ','); monster.TypeId = std::stoi(cell);
        std::getline(ss, cell, ','); monster.Level = std::stoi(cell);
        std::getline(ss, cell, ','); monster.Chapter = std::stoi(cell);
		std::getline(ss, cell, ','); monster.Name = cell;
        std::getline(ss, cell, ','); monster.Hp = std::stoi(cell);
        std::getline(ss, cell, ','); monster.Atk = std::stoi(cell);
        std::getline(ss, cell, ','); monster.CrtRate = std::stof(cell);
        std::getline(ss, cell, ','); monster.MoveSpd = std::stof(cell);
        std::getline(ss, cell, ','); monster.Dodge = std::stof(cell);
        std::getline(ss, cell, ','); monster.CollisionRadius = std::stof(cell);
        std::getline(ss, cell, ','); monster.AtkRadius = std::stof(cell);

        _monsterMap[monster.TypeId] = monster;
        _chapterMap[monster.Chapter].push_back(monster);
    }

    return true;
}

const FMonsterData* MonsterTable::GetMonsterByTypeId(uint32 typeId) const
{
    auto it = _monsterMap.find(typeId);
    if (it != _monsterMap.end())
        return &it->second;
    return nullptr;
}

const std::vector<FMonsterData>& MonsterTable::GetMonstersByChapter(uint32 chapter) const
{
    static std::vector<FMonsterData> empty;
    auto it = _chapterMap.find(chapter);
    return (it != _chapterMap.end()) ? it->second : empty;
}
