#include "pch.h"
#include "SpawnTable.h"

bool SpawnTable::LoadFromCSV(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open spawn CSV: " << filePath << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;

        SpawnInfo spawn;

        std::getline(ss, cell, ','); spawn.Zone = ParseZoneType(cell);
        std::getline(ss, cell, ','); spawn.MonsterType = ParseMonsterType(cell);
        std::getline(ss, cell, ','); spawn.Count = std::stoi(cell);
        std::getline(ss, cell, ','); spawn.SpawnPos.X = std::stof(cell);
        std::getline(ss, cell, ','); spawn.SpawnPos.Y = std::stof(cell);
        std::getline(ss, cell, ','); spawn.SpawnPos.Z = std::stof(cell);
        std::getline(ss, cell, ','); spawn.SpawnDelaySec = std::stof(cell);
        std::getline(ss, cell, ','); spawn.DropItemID = ParseDropItem(cell);
        std::getline(ss, cell, ','); spawn.bIsBoss = (cell == "1");

        _spawnMap[spawn.Zone].push_back(spawn);
    }

    return true;
}

const std::vector<SpawnInfo>& SpawnTable::GetSpawnsByZone(ZoneType zone) const
{
    static std::vector<SpawnInfo> empty;
    auto it = _spawnMap.find(zone);
    return (it != _spawnMap.end()) ? it->second : empty;
}

ZoneType SpawnTable::ParseZoneType(const std::string& str)
{
    if (str == "TIP") return ZoneType::TIP;
    if (str == "TUK") return ZoneType::TUK;
    if (str == "E") return ZoneType::E;
    if (str == "GYM") return ZoneType::GYM;
    if (str == "INDUSTY") return ZoneType::INDUSTY;
    if (str == "BUNKER") return ZoneType::BUNKER;
    if (str == "PLAYGROUND") return ZoneType::PLAYGROUND;
    return ZoneType::NONE;
}

Type::EMonster SpawnTable::ParseMonsterType(const std::string& str)
{
    using M = Type::EMonster;
    if (str == "ENERGY_DRINK") return M::ENERGY_DRINK;
    if (str == "BUBBLE_TEA") return M::BUBBLE_TEA;
    if (str == "COFFEE") return M::COFFEE;
    if (str == "MOUSE") return M::MOUSE;
    if (str == "KEYBOARD") return M::KEYBOARD;
    if (str == "DESKMON") return M::DESKMON;
    if (str == "COGWHEEL") return M::COGWHEEL;
    if (str == "BOLT_NUT") return M::BOLT_NUT;
    if (str == "DRILL") return M::DRILL;
    if (str == "TINO") return M::TINO;
    return static_cast<M>(0);
}

int32 SpawnTable::ParseDropItem(const std::string& str)
{
    if (str == "ENERGY_SWORD") return static_cast<int32>(Type::EWeapon::ENERGY_SWORD);
    if (str == "COFFEE") return static_cast<int32>(Type::EUseable::COFFEE);
    if (str == "KEY") return static_cast<int32>(Type::EQuestItem::KEY);
    return -1;
}