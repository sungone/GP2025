#include "pch.h"
#include "PlayerLevelTable.h"

bool PlayerLevelTable::LoadFromCSV(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open level stat CSV: " << filePath << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;

        FStatData stat;

        std::getline(ss, cell, ','); stat.Level = std::stoi(cell);
        std::getline(ss, cell, ','); stat.MaxHp = std::stof(cell);
        std::getline(ss, cell, ','); stat.Damage = std::stof(cell);
        std::getline(ss, cell, ','); stat.CrtRate = std::stof(cell);
        std::getline(ss, cell, ','); stat.CrtValue = std::stof(cell);
        std::getline(ss, cell, ','); stat.Dodge = std::stof(cell);
        std::getline(ss, cell, ','); stat.MaxExp = std::stof(cell);

        stat.Hp = stat.MaxHp;

        _levelStats[stat.Level] = stat;
    }

    file.close();
    return true;
}

const FStatData* PlayerLevelTable::GetStatByLevel(uint32 level) const
{
    auto it = _levelStats.find(level);
    if (it != _levelStats.end())
    {
        return &it->second;
    }
    return nullptr;
}
