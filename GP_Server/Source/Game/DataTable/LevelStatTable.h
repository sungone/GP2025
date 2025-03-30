#pragma once

class LevelStatTable
{
public:
    static LevelStatTable& GetInst()
    {
        static LevelStatTable inst;
        return inst;
    }

    bool LoadFromCSV(const std::string& filePath);
    const FStatData* GetStatByLevel(uint32 level) const;

private:
    std::unordered_map<uint32, FStatData> _levelStats;
};
