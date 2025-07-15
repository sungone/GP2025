#pragma once
#include "Common.h"

enum class DBResultCode : int32
{
	SUCCESS = 0,
	INVALID_USER = -1,
	INVALID_PASSWORD = -2,
	DUPLICATE_ID = -3,

	//Shop
	NOT_ENOUGH_GOLD = -10,
	ITEM_NOT_FOUND = -11,

	// Friend
	FRIEND_ALREADY_REQUESTED = -20,
	FRIEND_ALREADY_ADDED = -21,
	FRIEND_SELF_REQUEST = -22,
	FRIEND_USER_NOT_FOUND = -23,

	DB_ERROR = -99
};

#pragma pack(push,1)

struct FSkillData
{
	ESkillGroup SkillGID;
	int32 SkillLevel;

	FSkillData()
		: SkillGID(ESkillGroup::None), SkillLevel(0)
	{
	}

	FSkillData(ESkillGroup group, int32 level)
		: SkillGID(group), SkillLevel(level)
	{
	}

	bool IsValid() const
	{
		return SkillGID != ESkillGroup::None && SkillLevel > 0;
	}

	void SetSkill(ESkillGroup group, int32 level)
	{
		SkillGID = group;
		SkillLevel = level;
	}

	void LevelUp()
	{
		if (SkillLevel < 3)
			++SkillLevel;
	}

	bool operator==(const FSkillData& Other) const
	{
		return SkillGID == Other.SkillGID && SkillLevel == Other.SkillLevel;
	}
#ifndef SERVER_BUILD
	friend uint32 GetTypeHash(const FSkillData& Data)
	{
		return HashCombine(GetTypeHash(static_cast<uint32>(Data.SkillGID)), GetTypeHash(Data.SkillLevel));
	}
#endif
};


struct FStatData
{
	uint32 Level;
	float Exp;
	float MaxExp;
	float Hp;
	float MaxHp;
	float Damage;
	float CrtRate;
	float CrtValue;
	float Dodge;
	float Speed;

	FStatData()
		: Level(1), Exp(0.0f), MaxExp(100.0f),
		Hp(100.0f), MaxHp(100.0f),
		Damage(10.0f), CrtRate(0.5f), CrtValue(1.5f),
		Dodge(0.1f), Speed(0.0f)
	{
	}
};

struct FSkillState
{
	FSkillData Q{};
	FSkillData E{};
	FSkillData R{};
};

struct FEquitState
{
	Type::EWeapon Sword = Type::EWeapon::NONE;
	Type::EArmor Helmet = Type::EArmor::NONE;
	Type::EArmor Chest = Type::EArmor::NONE;
};

struct QuestStatus
{
	QuestType QuestType = QuestType::NONE;
	EQuestStatus Status = EQuestStatus::NotStarted;
};

struct ShopItemInfo
{
	uint8  ItemType;
	uint32 ItemID;
	uint32 Price;
};

struct FFriendInfo
{
	uint32 Id;
	char NickName[NICKNAME_LEN];
	uint32 Level;
	bool bAccepted;
	bool isOnline = false;
	const char* GetName() const { return NickName; }
#ifdef SERVER_BUILD
	void SetName(std::wstring InNick)
	{
		if (InNick.empty()) {
			SAFE_STRCPY(NickName, "None", NICKNAME_LEN - 1);
			NickName[NICKNAME_LEN - 1] = '\0';
			return;
		}

		int utf8Length = WideCharToMultiByte(CP_UTF8, 0, InNick.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (utf8Length <= 0) {
			SAFE_STRCPY(NickName, "Invalid", NICKNAME_LEN - 1);
			NickName[NICKNAME_LEN - 1] = '\0';
			return;
		}

		std::string utf8Str(utf8Length, 0);
		WideCharToMultiByte(CP_UTF8, 0, InNick.c_str(), -1, &utf8Str[0], utf8Length, nullptr, nullptr);

		SAFE_STRCPY(NickName, utf8Str.c_str(), NICKNAME_LEN - 1);
		NickName[NICKNAME_LEN - 1] = '\0';
	}
#endif
};
#define START_ZONE ZoneType::TUK
struct FInfoData
{
	int32 ID{};
	char NickName[NICKNAME_LEN];
	uint8 CharacterType;
	FVector Pos;
	float Yaw;
	float CollisionRadius;
	float AttackRadius;
	float fovAngle;

	FStatData Stats;
	uint32 State;
	uint32 Gold;
	FSkillState Skills;
	FEquitState EquipState;
	ZoneType CurrentZone{ START_ZONE };

	QuestStatus CurrentQuest;

	FInfoData()
		: ID(0),
		NickName("None"),
		CharacterType(),
		Pos(FVector(0.0f, 0.0f, 0.0f)),
		Yaw(0.0f),
		CollisionRadius(0.0f),
		AttackRadius(0.f),
		fovAngle(90.f),
		Stats(),
		State(STATE_IDLE),
		Gold(0)
	{
	}

	void SetYaw(float Yaw_) { Yaw = Yaw_; }
	void SetLocation(float X_, float Y_, float Z_) { Pos = FVector(X_, Y_, Z_); }
	void SetLocation(FVector Pos_) { Pos = Pos_; }
	void AddState(ECharacterStateType NewState) { State |= NewState; }
	void RemoveState(ECharacterStateType RemoveState) { State &= ~RemoveState; }
	bool HasState(ECharacterStateType CheckState) const { return (State & CheckState) != 0; }
	void SetZone(ZoneType zone) { CurrentZone = zone; }
	ZoneType GetZone() const { return CurrentZone; }
	int32 GetLevel() const { return Stats.Level; }
	float GetHp() const { return Stats.Hp; }
	float GetMaxHp() const { return Stats.MaxHp; }
	float GetExp() const { return Stats.Exp; }
	float GetMaxExp() const { return Stats.MaxExp; }
	float GetDamage() const { return Stats.Damage; }
	float GetCrtRate() const { return Stats.CrtRate; }
	float GetCrtValue() const { return Stats.CrtValue; }
	float GetDodge() const { return Stats.Dodge; }
	float GetSpeed() const { return Stats.Speed; }
	const char* GetName() const { return NickName; }
	FSkillData* GetSkillData(ESkillGroup groupId)
	{
		switch (groupId)
		{
		case ESkillGroup::Throwing:
		case ESkillGroup::HitHard:
			return &Skills.Q;

		case ESkillGroup::FThrowing:
		case ESkillGroup::Clash:
			return &Skills.E;

		case ESkillGroup::Anger:
		case ESkillGroup::Whirlwind:
			return &Skills.R;

		default:
			return nullptr;
		}
	}
	int32 GetSkillLevel(ESkillGroup groupId)
	{
		FSkillData* skill = GetSkillData(groupId);
		return skill ? skill->SkillLevel : -1;
	}
	void EquipWeapon(Type::EWeapon Weapon)
	{
		if (Weapon != Type::EWeapon::NONE)
			EquipState.Sword = Weapon;
	}

	void EquipItemByType(uint8 itemTypeID)
	{
		if (itemTypeID >= static_cast<uint8>(Type::EWeapon::START) &&
			itemTypeID < static_cast<uint8>(Type::EWeapon::END))
		{
			EquipState.Sword = static_cast<Type::EWeapon>(itemTypeID);
		}
		else if (itemTypeID >= static_cast<uint8>(Type::EArmor::START) &&
			itemTypeID < static_cast<uint8>(Type::EArmor::END))
		{
			Type::EArmor armor = static_cast<Type::EArmor>(itemTypeID);
			switch (armor)
			{
			case Type::EArmor::ALLOY_HELMET:
			case Type::EArmor::ENHANCED_HELMET:
				EquipState.Helmet = armor;
				break;

			case Type::EArmor::TITANIUM_ARMOR:
			case Type::EArmor::POWERED_ARMOR:
			case Type::EArmor::SUIT:
			case Type::EArmor::TUCLOTHES:
				EquipState.Chest = armor;
				break;

			default:
				break;
			}
		}
	}

	void UnequipItemByType(uint8 itemTypeID)
	{
		if (itemTypeID >= static_cast<uint8>(Type::EWeapon::START) &&
			itemTypeID < static_cast<uint8>(Type::EWeapon::END))
		{
			EquipState.Sword = Type::EWeapon::NONE;
		}
		else if (itemTypeID >= static_cast<uint8>(Type::EArmor::START) &&
			itemTypeID < static_cast<uint8>(Type::EArmor::END))
		{
			Type::EArmor armor = static_cast<Type::EArmor>(itemTypeID);
			switch (armor)
			{
			case Type::EArmor::ALLOY_HELMET:
			case Type::EArmor::ENHANCED_HELMET:
				EquipState.Helmet = Type::EArmor::NONE;
				break;

			case Type::EArmor::TITANIUM_ARMOR:
			case Type::EArmor::POWERED_ARMOR:
			case Type::EArmor::SUIT:
			case Type::EArmor::TUCLOTHES:
				EquipState.Chest = Type::EArmor::NONE;
				break;

			default:
				break;
			}
		}
	}
	Type::EWeapon GetEquippedWeapon() const { return EquipState.Sword; }
	Type::EArmor  GetEquippedHelmet() const { return EquipState.Helmet; }
	Type::EArmor  GetEquippedChest() const { return EquipState.Chest; }


	const QuestStatus& GetCurrentQuest() const { return CurrentQuest; }

#ifdef SERVER_BUILD
	void SetHp(float NewHp) { Stats.Hp = std::clamp(NewHp, 0.0f, Stats.MaxHp); }
	void Heal(float Amount) { SetHp(Stats.Hp + Amount); }
	void TakeDamage(float Amount) { SetHp(Stats.Hp - Amount); }
	bool IsDead() const { return Stats.Hp < 1.0f; }
	void SetDamage(float NewDamage) { Stats.Damage = std::max(0.0f, NewDamage); }
	float GetAttackDamage(float RandomValue) const
	{
		if (RandomValue < 0.1f)
			return 0.0f;

		bool bIsCritical = RandomValue < Stats.CrtRate;
		return bIsCritical ? Stats.Damage * Stats.CrtValue : Stats.Damage;
	}
	void SetLocationAndYaw(FVector NewPos)
	{
		Yaw = CalculateYaw(NewPos);
		Pos = NewPos;
	}
	float CalculateYaw(FVector TargetPos) const
	{
		return Pos.GetYawToTarget(TargetPos);
	}
	FVector GetFrontVector() const
	{
		float RadianYaw = Yaw * (3.14159265359f / 180.0f);
		float ForwardX = std::cos(RadianYaw);
		float ForwardY = std::sin(RadianYaw);
		return FVector(ForwardX, ForwardY, 0.0f).Normalize();
	}
	void SetName(std::wstring InNick)
	{
		if (InNick.empty()) {
			SAFE_STRCPY(NickName, "None", NICKNAME_LEN - 1);
			NickName[NICKNAME_LEN - 1] = '\0';
			return;
		}

		int utf8Length = WideCharToMultiByte(CP_UTF8, 0, InNick.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (utf8Length <= 0) {
			SAFE_STRCPY(NickName, "Invalid", NICKNAME_LEN - 1);
			NickName[NICKNAME_LEN - 1] = '\0';
			return;
		}

		std::string utf8Str(utf8Length, 0);
		WideCharToMultiByte(CP_UTF8, 0, InNick.c_str(), -1, &utf8Str[0], utf8Length, nullptr, nullptr);

		SAFE_STRCPY(NickName, utf8Str.c_str(), NICKNAME_LEN - 1);
		NickName[NICKNAME_LEN - 1] = '\0';
	}

#endif
};

#pragma pack(pop)