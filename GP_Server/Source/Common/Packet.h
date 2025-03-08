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

struct AttackPacket : public Packet
{
	int32 TargetID;

	AttackPacket(int32 TargetID)
		: Packet(EPacketType::C_ATTACK), TargetID(TargetID)
	{
		Header.PacketSize = sizeof(AttackPacket);
	}
};
struct DamagePacket : public Packet
{
	FInfoData Target;
	float Damage;

	DamagePacket(const FInfoData& Target_, float Damage_)
		: Packet(EPacketType::S_DAMAGED_MONSTER), Target(Target_), Damage(Damage_)
	{
		Header.PacketSize = sizeof(DamagePacket);
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
			: Packet(EPacketType::S_ADD_IVENTORY_ITEM), ItemID(itemId),ItemType(type)
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
		EquipItemPacket()
			: Packet(EPacketType::S_EQUIP_ITEM)
		{
			Header.PacketSize = sizeof(EquipItemPacket);
		}
	};

	struct UnequipItemPacket : public Packet
	{
		UnequipItemPacket()
			: Packet(EPacketType::S_UNEQUIP_ITEM)
		{
			Header.PacketSize = sizeof(UnequipItemPacket);
		}
	};
}// namespace Item
#pragma pack(pop)
