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

struct ItemSpawnPacket : public Packet
{
	uint32 ItemID;
	EItem ItemType;
	FVector Pos;

	ItemSpawnPacket(uint32 id, EItem type, FVector pos)
		: Packet(EPacketType::S_ITEM_SPAWN), ItemID(id), ItemType(type), Pos(pos)
	{
		Header.PacketSize = sizeof(ItemSpawnPacket);
	}
};

struct ItemDespawnPacket : public Packet
{
	uint32 ItemID;
	ItemDespawnPacket(uint32 id)
		: Packet(EPacketType::S_ITEM_DESPAWN), ItemID(id)
	{
		Header.PacketSize = sizeof(ItemDespawnPacket);
	}
};

struct AddInventoryItemPacket : public Packet
{
	EItem ItemType;
	uint32 Quantity;

	AddInventoryItemPacket(EItem type, uint32 quantity)
		: Packet(EPacketType::S_ADD_IVENTORY_ITEM), ItemType(type), Quantity(quantity)
	{
		Header.PacketSize = sizeof(AddInventoryItemPacket);
	}
};

struct RemoveInventoryItemPacket : public Packet
{
	EItem ItemType;
	uint32 Quantity;

	RemoveInventoryItemPacket(EItem type, uint32 quantity)
		: Packet(EPacketType::S_REMOVE_IVENTORY_ITEM), ItemType(type), Quantity(quantity)
	{
		Header.PacketSize = sizeof(RemoveInventoryItemPacket);
	}
};

#pragma pack(pop)