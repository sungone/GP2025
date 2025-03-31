#pragma once

enum class ESkillType
{
    None,
    Atk,
    Dash,
    RangeAtk,
    SectorAtk,
    BuffTime,
    AtkSpd
};

struct FSkillData
{
    uint32 TypeId;
    std::string Name;
    Type::EPlayer WeaponType;
    uint32 SkillGroup;
    uint32 SkillLv;
    uint32 Cooltime;

    ESkillType Type0 = ESkillType::None;
    float Value0 = 0.f;

    ESkillType Type1 = ESkillType::None;
    float Value1 = 0.f;
};

class PlayerSkillTable
{
public:
    static PlayerSkillTable& GetInst()
    {
        static PlayerSkillTable inst;
        return inst;
    }

    bool LoadFromCSV(const std::string& filePath);
    const FSkillData* GetSkill(uint32 idx) const;
    const std::vector<FSkillData>& GetSkillGroup(uint32 groupId) const;
    const std::vector<FSkillData>& GetSkillsByWeaponType(Type::EPlayer weapon) const;
private:
    std::unordered_map<uint32, FSkillData> _skillMap;
    std::unordered_map<uint32, std::vector<FSkillData>> _groupMap;
    std::unordered_map<Type::EPlayer, std::vector<FSkillData>> _weaponMap;
    ESkillType StringToSkillType(const std::string& str);
    Type::EPlayer StringToWeaponType(const std::string& str);
};
