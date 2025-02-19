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

struct AddItemPacket : public Packet
{
	FItem Item;

	AddItemPacket(const FItem& item)
		: Packet(EPacketType::S_ADD_ITEM), Item(item)
	{
		Header.PacketSize = sizeof(AddItemPacket);
	}
};

struct RemoveItemPacket : public Packet
{
	uint32 SlotIndex;
	uint32 Quantity;

	RemoveItemPacket(uint32 slotIndex, uint32 quantity)
		: Packet(EPacketType::S_REMOVE_ITEM), SlotIndex(slotIndex), Quantity(quantity)
	{
		Header.PacketSize = sizeof(RemoveItemPacket);
	}
};

#pragma pack(pop)