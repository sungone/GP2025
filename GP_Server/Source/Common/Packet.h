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

constexpr int LOGIN_STR_LEN = 20 + 1;
constexpr int NICKNAME_LEN = 10 + 1;
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

struct SignupPacket : public Packet
{
	char AccountID[LOGIN_STR_LEN];
	char AccountPW[LOGIN_STR_LEN];
	char NickName[NICKNAME_LEN];

	SignupPacket(const char* InAccountID, const char* InAccountPW, const char* InNickName)
		: Packet(EPacketType::C_SIGNUP)
	{
		strncpy_s(AccountID, InAccountID, LOGIN_STR_LEN - 1);
		AccountID[LOGIN_STR_LEN - 1] = '\0';

		strncpy_s(AccountPW, InAccountPW, LOGIN_STR_LEN - 1);
		AccountPW[LOGIN_STR_LEN - 1] = '\0';

		strncpy_s(NickName, InNickName, NICKNAME_LEN - 1);
		NickName[NICKNAME_LEN - 1] = '\0';

		Header.PacketSize = sizeof(SignupPacket);
	}
};

struct LoginSuccessPacket : public Packet
{
	char NickName[NICKNAME_LEN];
	LoginSuccessPacket(const char* NickNamePW_)
		: Packet(EPacketType::S_LOGIN_SUCCESS)
	{
		strncpy_s(NickName, NickNamePW_, NICKNAME_LEN - 1);
		NickName[NICKNAME_LEN - 1] = '\0';

		Header.PacketSize = sizeof(LoginSuccessPacket);
	}
};

struct AttackPacket : public Packet
{
	float PlayerYaw;

	AttackPacket(float PlayerYaw_)
		: Packet(EPacketType::C_ATTACK), PlayerYaw(PlayerYaw_)
	{
		Header.PacketSize = sizeof(AttackPacket);
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
			: Packet(EPacketType::S_ADD_IVENTORY_ITEM), ItemID(itemId), ItemType(type)
		{
			Header.PacketSize = sizeof(AddInventoryPacket);
		}
	};

	struct RemoveInventoryPacket : public Packet
	{
		uint32 ItemID;

		RemoveInventoryPacket(uint32 itemId)
			: Packet(EPacketType::S_REMOVE_IVENTORY_ITEM), ItemID(itemId)
		{
			Header.PacketSize = sizeof(RemoveInventoryPacket);
		}
	};

	struct EquipItemPacket : public Packet
	{
		int32 PlayerID;
		uint8 ItemType;
		EquipItemPacket(int32 playerId, uint8 type)
			: Packet(EPacketType::S_EQUIP_ITEM), PlayerID(playerId), ItemType(type)
		{
			Header.PacketSize = sizeof(EquipItemPacket);
		}
	};

	struct UnequipItemPacket : public Packet
	{
		int32 PlayerID;
		uint8 ItemType;
		UnequipItemPacket(int32 playerId, uint8 type)
			: Packet(EPacketType::S_UNEQUIP_ITEM), PlayerID(playerId), ItemType(type)
		{
			Header.PacketSize = sizeof(UnequipItemPacket);
		}
	};
}// namespace Item
#pragma pack(pop)
