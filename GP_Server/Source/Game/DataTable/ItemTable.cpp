#include "pch.h"
#include "ItemTable.h"

bool ItemTable::LoadFromCSV(const std::string& FilePath)
{
    std::ifstream file(FilePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << FilePath << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;
        std::string dummy;
        FItemData item;

        std::getline(ss, cell, ','); item.TypeID = std::stoi(cell);
        std::getline(ss, dummy, ',');

        std::getline(ss, cell, ','); item.Category = StringToCategory(cell);
        std::getline(ss, cell, ','); item.Damage = std::stof(cell);
        std::getline(ss, cell, ','); item.Hp = std::stof(cell);
        std::getline(ss, cell, ','); item.CrtRate = std::stof(cell);
        std::getline(ss, cell, ','); item.DodgeRate = std::stof(cell);
        std::getline(ss, cell, ','); item.MoveSpeed = std::stof(cell);
        std::getline(ss, cell, ','); item.AbilityType = StringToAbilityType(cell);
        std::getline(ss, cell, ','); item.AbilityValue = std::stof(cell);
        std::getline(ss, cell, ','); item.DropRate = std::stof(cell);
        std::getline(ss, cell, ','); item.FromMonster = std::stoi(cell) == 1;
        std::getline(ss, cell, ','); item.ResellPrice = std::stoi(cell);
        std::getline(ss, cell, ','); item.Price = std::stoi(cell);
        std::getline(ss, cell, ','); item.bSellable = std::stoi(cell) == 1;
        std::getline(ss, cell, ','); item.bBuyable = std::stoi(cell) == 1;

        _itemMap[item.TypeID] = item;
    }

    file.close();
    return true;
}

const FItemData* ItemTable::GetItemByTypeId(uint32 TypeID) const
{
    auto it = _itemMap.find(TypeID);
    if (it != _itemMap.end())
    {
        return &it->second;
    }
    return nullptr;
}

EItemCategory ItemTable::StringToCategory(const std::string& str)
{
    if (str == "weapon") return EItemCategory::Weapon;
    if (str == "armor") return EItemCategory::Armor;
    if (str == "useable") return EItemCategory::Useable;
    if (str == "unuseable") return EItemCategory::Quest;
    return EItemCategory::Unknown;
}

EAbilityType ItemTable::StringToAbilityType(const std::string& str)
{
    if (str == "recove") return EAbilityType::Recove;
    if (str == "atc_buff") return EAbilityType::AtcBuff;
    if (str == "Gold") return EAbilityType::Gold;
    return EAbilityType::None;
}
