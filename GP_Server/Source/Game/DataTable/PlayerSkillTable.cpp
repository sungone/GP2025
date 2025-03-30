#include "pch.h"
#include "PlayerSkillTable.h"

bool PlayerSkillTable::LoadFromCSV(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open skill CSV: " << filePath << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // skip header

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;

        FSkillData skill;

        std::getline(ss, cell, ','); skill.Idx = std::stoi(cell);
        std::getline(ss, skill.Name, ',');
        std::getline(ss, cell, ','); skill.SkillGroup = std::stoi(cell);
        std::getline(ss, cell, ','); skill.SkillLv = std::stoi(cell);
        std::getline(ss, cell, ','); skill.Cooltime = std::stoi(cell);

        std::getline(ss, cell, ','); skill.Type0 = StringToSkillType(cell);
        std::getline(ss, cell, ','); skill.Value0 = std::stof(cell);

        std::getline(ss, cell, ','); skill.Type1 = StringToSkillType(cell);
        std::getline(ss, cell, ','); skill.Value1 = std::stof(cell);

        _skillMap[skill.Idx] = skill;
        _groupMap[skill.SkillGroup].push_back(skill);
    }

    return true;
}

const FSkillData* PlayerSkillTable::GetSkill(uint32 idx) const
{
    auto it = _skillMap.find(idx);
    if (it != _skillMap.end())
        return &it->second;
    return nullptr;
}


const std::vector<FSkillData>& PlayerSkillTable::GetSkillGroup(uint32 groupId) const
{
    static std::vector<FSkillData> empty;
    auto it = _groupMap.find(groupId);
    if (it != _groupMap.end())
        return it->second;
    return empty;
}


ESkillType PlayerSkillTable::StringToSkillType(const std::string& str)
{
    if (str == "atk") return ESkillType::Atk;
    if (str == "dash") return ESkillType::Dash;
    if (str == "range_atk") return ESkillType::RangeAtk;
    if (str == "sector_atk") return ESkillType::SectorAtk;
    if (str == "buff_time") return ESkillType::BuffTime;
    if (str == "atk_spd") return ESkillType::AtkSpd;
    return ESkillType::None;
}

