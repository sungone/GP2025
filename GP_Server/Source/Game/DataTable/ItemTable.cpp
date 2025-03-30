#include "pch.h"
#include "ItemTable.h"
#include <iomanip>

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
        std::getline(ss, dummy, ',');

        std::getline(ss, cell, ','); item.Damage = std::stof(cell);
        std::getline(ss, cell, ','); item.Hp = std::stof(cell);
        std::getline(ss, cell, ','); item.CrtRate = std::stof(cell);
        std::getline(ss, cell, ','); item.DodgeRate = std::stof(cell);
        std::getline(ss, cell, ','); item.MoveSpeed = std::stof(cell);

        std::getline(ss, cell, ','); item.AbilityType = StringToAbilityType(cell);
        std::getline(ss, cell, ','); item.AbilityValue = std::stof(cell);
        std::getline(ss, cell, ','); item.Grade = std::stoi(cell);
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

void ItemTable::PrintAllItems() const
{
    std::cout << "=== All Items in Item Table ===" << std::endl;
    std::cout << std::left
        << std::setw(5) << "ID"
        << std::setw(10) << "Category"
        << std::setw(8) << "Damage"
        << std::setw(8) << "Hp"
        << std::setw(10) << "CrtRate"
        << std::setw(12) << "DodgeRate"
        << std::setw(12) << "MoveSpeed"
        << std::setw(12) << "AbilityType"
        << std::setw(12) << "AbilityVal"
        << std::setw(7) << "Grade"
        << std::setw(8) << "Price"
        << std::setw(8) << "Resell"
        << std::setw(10) << "FromMon"
        << std::setw(10) << "Sellable"
        << std::setw(10) << "Buyable"
        << std::endl;

    std::cout << std::string(180, '-') << std::endl;

    for (const auto& [itemId, item] : _itemMap)
    {
        std::cout << std::left
            << std::setw(5) << item.TypeID
            << std::setw(10) << static_cast<int>(item.Category)
            << std::setw(8) << item.Damage
            << std::setw(8) << item.Hp
            << std::setw(10) << item.CrtRate
            << std::setw(12) << item.DodgeRate
            << std::setw(12) << item.MoveSpeed
            << std::setw(12) << static_cast<int>(item.AbilityType)
            << std::setw(12) << item.AbilityValue
            << std::setw(7) << item.Grade
            << std::setw(8) << item.Price
            << std::setw(8) << item.ResellPrice
            << std::setw(10) << (item.FromMonster ? "Yes" : "No")
            << std::setw(10) << (item.bSellable ? "Yes" : "No")
            << std::setw(10) << (item.bBuyable ? "Yes" : "No")
            << std::endl;
    }
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
