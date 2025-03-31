#pragma once

enum class EAbilityType { None, Recove, AtcBuff, Gold };
struct FItemData
{
	uint32 TypeId;
	EItemCategory Category;
	float Damage = 0.f;
	float Hp = 0.f;
	float CrtRate = 0.f;
	float DodgeRate = 0.f;
	float MoveSpeed = 0.f;
	EAbilityType AbilityType = EAbilityType::None;
	float AbilityValue = 0.f;
	float DropRate = 0.f;
	bool FromMonster = false;
	int ResellPrice = 0;
	int Price = 0;
	bool bSellable = false;
	bool bBuyable = false;
};

class ItemTable
{
public:
	static ItemTable& GetInst()
	{
		static ItemTable inst;
		return inst;
	}
	bool LoadFromCSV(const std::string& FilePath);
	const FItemData* GetItemByTypeId(uint32 TypeID) const;
private:
	std::unordered_map<uint32, FItemData> _itemMap;

	EItemCategory StringToCategory(const std::string& str);
	EAbilityType StringToAbilityType(const std::string& str);
};