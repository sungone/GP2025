#pragma once
#include "Common.h"

#pragma pack(push,1)
struct Packet
{
	struct PacketHeader
	{
		EPacketType PacketType;
		uint8_t PacketSize;

		PacketHeader(EPacketType type, uint8_t size)
			: PacketType(type), PacketSize(size) {
		}
	};

	PacketHeader Header;

	Packet(EPacketType type)
		: Header(type, sizeof(PacketHeader)) {
	}
};

template<typename T>
struct TPacket : public Packet
{
	T Data;
	TPacket(EPacketType type, const T& payload)
		: Packet(type), Data(payload)
	{
		Header.PacketSize = sizeof(PacketHeader) + sizeof(T);
	}
};

using FPacketHeader = Packet::PacketHeader;
using InfoPacket = TPacket<FInfoData>;
using IDPacket = TPacket<int32>;
#pragma region Account
struct LoginPacket : public Packet
{
	char AccountID[LOGIN_STR_LEN];
	char AccountPW[LOGIN_STR_LEN];
	LoginPacket(const char* AccountID_, const char* AccountPW_)
		: Packet(EPacketType::C_LOGIN)
	{
		strncpy_s(AccountID, AccountID_, LOGIN_STR_LEN - 1);
		AccountID[LOGIN_STR_LEN - 1] = '\0';

		strncpy_s(AccountPW, AccountPW_, LOGIN_STR_LEN - 1);
		AccountPW[LOGIN_STR_LEN - 1] = '\0';

		Header.PacketSize = sizeof(LoginPacket);
	}
};

struct SignUpPacket : public Packet
{
	char AccountID[LOGIN_STR_LEN];
	char AccountPW[LOGIN_STR_LEN];
	char NickName[NICKNAME_LEN];

	SignUpPacket(const char* InAccountID, const char* InAccountPW, const char* InNickName)
		: Packet(EPacketType::C_SIGNUP)
	{
		strncpy_s(AccountID, InAccountID, LOGIN_STR_LEN - 1);
		AccountID[LOGIN_STR_LEN - 1] = '\0';

		strncpy_s(AccountPW, InAccountPW, LOGIN_STR_LEN - 1);
		AccountPW[LOGIN_STR_LEN - 1] = '\0';

		strncpy_s(NickName, InNickName, NICKNAME_LEN - 1);
		NickName[NICKNAME_LEN - 1] = '\0';

		Header.PacketSize = sizeof(SignUpPacket);
	}
};

struct LoginSuccessPacket : public Packet
{
	bool dummy;
	LoginSuccessPacket()
		: Packet(EPacketType::S_LOGIN_SUCCESS)
	{
		Header.PacketSize = sizeof(LoginSuccessPacket);
	}
};

struct LoginFailPacket : public Packet
{
	DBResultCode ResultCode;
	LoginFailPacket(const DBResultCode ResultCode_)
		: Packet(EPacketType::S_LOGIN_FAIL), ResultCode(ResultCode_)
	{
		Header.PacketSize = sizeof(LoginFailPacket);
	}
};

struct SignUpSuccessPacket : public Packet
{
	bool dummy;
	SignUpSuccessPacket()
		: Packet(EPacketType::S_SIGNUP_SUCCESS)
	{
		Header.PacketSize = sizeof(SignUpSuccessPacket);
	}
};

struct SignUpFailPacket : public Packet
{
	DBResultCode ResultCode;
	SignUpFailPacket(const DBResultCode ResultCode_)
		: Packet(EPacketType::S_SIGNUP_FAIL), ResultCode(ResultCode_)
	{
		Header.PacketSize = sizeof(SignUpFailPacket);
	}
};
#pragma endregion

struct SelectCharacterPacket : public Packet
{
	Type::EPlayer PlayerType;
	SelectCharacterPacket(Type::EPlayer PlayerType_)
		: Packet(EPacketType::C_SELECT_CHARACTER), PlayerType(PlayerType_)
	{
		Header.PacketSize = sizeof(SelectCharacterPacket);
	}
};
struct RequestEnterGamePacket : public Packet
{
	Type::EPlayer PlayerType;
	RequestEnterGamePacket(Type::EPlayer InType = Type::EPlayer::NONE)
		: Packet(EPacketType::C_ENTER_GAME), PlayerType(InType)
	{
		Header.PacketSize = sizeof(RequestEnterGamePacket);
	}
};
struct EnterGamePacket : public Packet
{
	FInfoData PlayerInfo;

	EnterGamePacket(const FInfoData& PlayerInfo_)
		: Packet(EPacketType::S_ENTER_GAME), PlayerInfo(PlayerInfo_)
	{
		Header.PacketSize = sizeof(EnterGamePacket);
	}
};

struct MovePacket : public Packet
{
	int32 PlayerID;
	FVector PlayerPos;
	uint32 State;
	uint64 MoveTime;

	MovePacket(int32 PlayerID_, const FVector& PlayerPos_, uint32 State_,
		uint64 SendTime_ = 0, EPacketType Type_ = EPacketType::C_MOVE)
		: Packet(Type_), PlayerID(PlayerID_), State(State_), PlayerPos(PlayerPos_), MoveTime(SendTime_)
	{
		Header.PacketSize = sizeof(MovePacket);
	}
};
struct PlayerLevelUpPacket : public Packet
{
	FInfoData PlayerInfo;
	PlayerLevelUpPacket(const FInfoData& PlayerInfo_)
		: Packet(EPacketType::S_LEVEL_UP), PlayerInfo(PlayerInfo_)
	{
		Header.PacketSize = sizeof(PlayerLevelUpPacket);
	}
};
#pragma region Attack
struct AttackPacket : public Packet
{
	float PlayerYaw;

	AttackPacket(float PlayerYaw_)
		: Packet(EPacketType::C_ATTACK), PlayerYaw(PlayerYaw_)
	{
		Header.PacketSize = sizeof(AttackPacket);
	}
};
struct StartAimingPacket : public Packet
{
	float PlayerYaw;
	StartAimingPacket(float PlayerYaw_)
		: Packet(EPacketType::C_START_AIMING), PlayerYaw(PlayerYaw_)
	{
		Header.PacketSize = sizeof(StartAimingPacket);
	}
};
struct StopAimingPacket : public Packet
{
	bool dummy;
	StopAimingPacket()
		: Packet(EPacketType::C_STOP_AIMING)
	{
		Header.PacketSize = sizeof(StopAimingPacket);
	}
};
struct MonsterDamagePacket : public Packet
{
	FInfoData Target;
	float Damage;

	MonsterDamagePacket(const FInfoData& Target_, float Damage_)
		: Packet(EPacketType::S_DAMAGED_MONSTER), Target(Target_), Damage(Damage_)
	{
		Header.PacketSize = sizeof(MonsterDamagePacket);
	}
};
struct PlayerDamagePacket : public Packet
{
	FInfoData Target;

	PlayerDamagePacket(const FInfoData& Target_)
		: Packet(EPacketType::S_DAMAGED_MONSTER), Target(Target_)
	{
		Header.PacketSize = sizeof(PlayerDamagePacket);
	}
};
#pragma endregion

#pragma region Skill
struct UseSkillPacket : public Packet
{
	ESkillGroup SkillGID;
	float PlayerYaw;

	UseSkillPacket(ESkillGroup SkillGID_, float PlayerYaw_)
		: Packet(EPacketType::C_USE_SKILL), SkillGID(SkillGID_), PlayerYaw(PlayerYaw_)
	{
		Header.PacketSize = sizeof(UseSkillPacket);
	}
};
struct SkillUnlockPacket : public Packet
{
	ESkillGroup SkillGID;
	SkillUnlockPacket(ESkillGroup SkillGID_)
		: Packet(EPacketType::S_SKILL_UNLOCK), SkillGID(SkillGID_)
	{
		Header.PacketSize = sizeof(SkillUnlockPacket);
	}
};
struct UpgradeSkillPacket : public Packet
{
	ESkillGroup SkillGID;
	UpgradeSkillPacket(ESkillGroup SkillGID_)
		: Packet(EPacketType::S_SKILL_UPGRADE), SkillGID(SkillGID_)
	{
		Header.PacketSize = sizeof(UpgradeSkillPacket);
	}
};
struct PlayerUseSkillPacket : public Packet
{
	int32 PlayerID;
	ESkillGroup SkillGID;

	PlayerUseSkillPacket(int32 PlayerID_, ESkillGroup SkillGID_)
		: Packet(EPacketType::S_PLAYER_USE_SKILL), PlayerID(PlayerID_), SkillGID(SkillGID_)
	{
		Header.PacketSize = sizeof(UseSkillPacket);
	}
};

struct PlayerDeadPacket : public Packet
{
	int32 PlayerID;

	PlayerDeadPacket(int32 playerId)
		: Packet(EPacketType::S_PLAYER_DEAD), PlayerID(playerId)
	{
		Header.PacketSize = sizeof(PlayerDeadPacket);
	}
};

#pragma endregion

namespace ItemPkt
{
	struct SpawnPacket : public Packet
	{
		uint32 ItemID;
		uint8 ItemType;
		FVector Pos;

		SpawnPacket(uint32 itemId, uint8 type, FVector pos)
			: Packet(EPacketType::S_ITEM_SPAWN), ItemID(itemId), ItemType(type), Pos(pos)
		{
			Header.PacketSize = sizeof(SpawnPacket);
		}
	};

	struct DespawnPacket : public Packet
	{
		uint32 ItemID;
		DespawnPacket(uint32 itemId)
			: Packet(EPacketType::S_ITEM_DESPAWN), ItemID(itemId)
		{
			Header.PacketSize = sizeof(DespawnPacket);
		}
	};

	struct PickUpPacket : public Packet
	{
		uint32 ItemID;
		PickUpPacket(uint32 itemId)
			: Packet(EPacketType::S_ITEM_PICKUP), ItemID(itemId)
		{
			Header.PacketSize = sizeof(PickUpPacket);
		}
	};

	struct DropPacket : public Packet
	{
		uint32 ItemID;
		uint8 ItemType;
		FVector Pos;

		DropPacket(uint32 itemId, uint8 type, FVector pos)
			: Packet(EPacketType::S_ITEM_SPAWN), ItemID(itemId), ItemType(type), Pos(pos)
		{
			Header.PacketSize = sizeof(DropPacket);
		}
	};

	struct AddInventoryPacket : public Packet
	{
		uint32 ItemID;
		uint8 ItemType;

		AddInventoryPacket(uint32 itemId, uint8 type)
			: Packet(EPacketType::S_ADD_INVENTORY_ITEM), ItemID(itemId), ItemType(type)
		{
			Header.PacketSize = sizeof(AddInventoryPacket);
		}
	};

	struct ItemUsedPacket : public Packet
	{
		uint32 ItemID;
		FInfoData PlayerInfo;

		ItemUsedPacket(uint32 itemId, const FInfoData& PlayerInfo_)
			: Packet(EPacketType::S_USE_INVENTORY_ITEM), ItemID(itemId), PlayerInfo(PlayerInfo_)
		{
			Header.PacketSize = sizeof(ItemUsedPacket);
		}
	};

	struct EquipItemPacket : public Packet
	{
		int32 PlayerID;
		uint8 ItemType;
		FInfoData PlayerInfo;

		EquipItemPacket(int32 playerId, uint8 type, const FInfoData& PlayerInfo_)
			: Packet(EPacketType::S_EQUIP_ITEM), PlayerID(playerId), ItemType(type), PlayerInfo(PlayerInfo_)
		{
			Header.PacketSize = sizeof(EquipItemPacket);
		}
	};

	struct UnequipItemPacket : public Packet
	{
		int32 PlayerID;
		uint8 ItemType;
		FInfoData PlayerInfo;

		UnequipItemPacket(int32 playerId, uint8 type, const FInfoData& PlayerInfo_)
			: Packet(EPacketType::S_UNEQUIP_ITEM), PlayerID(playerId), ItemType(type), PlayerInfo(PlayerInfo_)
		{
			Header.PacketSize = sizeof(UnequipItemPacket);
		}
	};
}// namespace Item

struct RequestZoneChangePacket : public Packet
{
	ZoneType TargetZone;

	RequestZoneChangePacket(ZoneType zone)
		: Packet(EPacketType::C_CHANGE_ZONE), TargetZone(zone)
	{
		Header.PacketSize = sizeof(RequestZoneChangePacket);
	}
};
struct ChangeZonePacket : public Packet
{
	ZoneType TargetZone;
	FVector RandomPos;

	ChangeZonePacket(ZoneType zone, const FVector& pos)
		: Packet(EPacketType::S_CHANGE_ZONE), TargetZone(zone), RandomPos(pos)
	{
		Header.PacketSize = sizeof(ChangeZonePacket);
	}
};
struct RespawnRequestPacket : public Packet
{
	ZoneType TargetZone;

	RespawnRequestPacket(ZoneType zone)
		: Packet(EPacketType::C_RESPAWN), TargetZone(zone)
	{
		Header.PacketSize = sizeof(RespawnRequestPacket);
	}
};
struct RespawnPacket : public Packet
{
	FInfoData PlayerInfo;

	RespawnPacket(const FInfoData& info)
		: Packet(EPacketType::S_RESPAWN), PlayerInfo(info)
	{
		Header.PacketSize = sizeof(RespawnPacket);
	}
};
#pragma pack(pop)
