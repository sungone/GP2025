#pragma once

struct FMonsterData
{
    uint32 TypeId;
    std::string Name;
    uint32 Chapter;
    int Level;
    int Hp;
    int Atk;
    float CrtRate;
    float MoveSpd;
    float Dodge;
    float CollisionRadius;
    float AtkRadius;
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
    const FMonsterData* GetMonsterByTypeId(uint32 typeId) const;
    const std::vector<FMonsterData>& GetMonstersByChapter(uint32 chapter) const;

private:
    std::unordered_map<uint32, FMonsterData> _monsterMap;
    std::unordered_map<uint32, std::vector<FMonsterData>> _chapterMap;
};
