#pragma once

class PlayerLevelTable
{
public:
    static PlayerLevelTable& GetInst()
    {
        static PlayerLevelTable inst;
        return inst;
    }

    bool LoadFromCSV(const std::string& filePath);
    const FStatData* GetStatByLevel(uint32 level) const;

private:
    std::unordered_map<uint32, FStatData> _levelStats;
};
