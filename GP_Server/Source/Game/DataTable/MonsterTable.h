#pragma once

struct FMonsterData
{
    uint32 TypeId;
    uint32 Chapter;
    int Hp;
    int Atk;
    float CrtRate;
    float MoveSpd;
    float Dodge;
};

class MonsterTable
{
public:
    static MonsterTable& GetInst()
    {
        static MonsterTable inst;
        return inst;
    }

    bool LoadFromCSV(const std::string& filePath);
    const FMonsterData* GetMonsterById(uint32 typeId) const;
    const std::vector<FMonsterData>& GetMonstersByChapter(uint32 chapter) const;

private:
    std::unordered_map<uint32, FMonsterData> _monsterMap;
    std::unordered_map<uint32, std::vector<FMonsterData>> _chapterMap;
};
