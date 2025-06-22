#pragma once

struct SpawnInfo
{
    ZoneType Zone;
    Type::EMonster MonsterType;
    int Count;
    FVector SpawnPos;
    float SpawnDelaySec;
    int32 DropItemID;
    bool bIsBoss;
};

class SpawnTable
{
public:
    bool LoadFromCSV(const std::string& filePath);

    const std::vector<SpawnInfo>& GetSpawnsByZone(ZoneType zone) const;

private:
    std::unordered_map<ZoneType, std::vector<SpawnInfo>> _spawnMap;
    ZoneType ParseZoneType(const std::string& str); // string ¡æ enum
    Type::EMonster ParseMonsterType(const std::string& str);
    int32 ParseDropItem(const std::string& str);
};