#pragma once
constexpr short SERVER_PORT = 4000;
constexpr int BUFSIZE = 1024;

using int32 = int;

enum EPacketType : uint8_t
{
	C_LOGIN,
	C_LOGOUT,
	C_MOVE,
	C_ATTACK,

	S_LOGIN_SUCCESS,

	S_ADD_PLAYER,
	S_REMOVE_PLAYER,
	S_PLAYER_STATUS_UPDATE,

	S_ADD_MONSTER,
	S_REMOVE_MONSTER,
	S_MONSTER_STATUS_UPDATE,
};

enum ECharacterType : uint8_t
{
	//INVALID = -1,

	P_WARRIOR,
	P_GUNNER,

	M_MOUSE,
};

#pragma region ObjectType

namespace CharacterType
{
	enum class ECharacterType : uint8_t
	{
		PLAYER,
		MONSTER,
		NPC
	};

	enum class EPlayerType : uint8_t
	{
		WARRIOR,
		GUNNER
	};

	enum class EMonsterType : uint8_t
	{
		ENERGY_DRINK,
		BUBBLE_TEA,
		COFFEE,
		MOUSE,
		KEYBOARD,
		DESKTOP,
		COGWHEEL,
		BOLT_NUT,
		DRILL,
		TINO
	};

	enum class ENPCType : uint8_t
	{
		PROFESSOR,
		GUARD,
		STUDENT
	};
}

namespace ItemType
{
	enum class EItemType : uint8_t
	{
		WEAPON,
		ARMOR,
		CONSUMABLE,
	};

	enum class EWeaponType : uint8_t
	{
		BIRD_GUN,
		PULSE_GUN,
		POSITRON,
		PRACS_WORD,
		PULSE_SWORD,
		ENERGY_SWORD,
	};

	enum class EArmorType : uint8_t
	{
		ALLOY_HELMET,
		ENHANCED_HELMET,
		TITANIUM_ARMOR,
		POWERED_ARMOR,

		SUIT,
		TUCLOTHES
	};

	enum class EConsumableType : uint8_t
	{
		HPKIT_LOW,
		HPKIT_MID,
		HPKIT_HIGH,

		COFFEE,
		DDALBA,
		MANGBA,
	};
}
#pragma endregion // ���� �̷��� �ٲٷ���

enum ECharacterStateType : uint32_t
{
	STATE_NONE = 0, // 2^0
	STATE_IDLE = 1 << 0,  // 2^1
	STATE_RUN = 1 << 1,  // 2^2
	STATE_JUMP = 1 << 2,  // 2^3
	STATE_AUTOATTACK = 1 << 3, // 2^4
	STATE_DIE = 1 << 4,
	STATE_WALK = 1 << 5,
};

struct FInfoData
{
	int32 ID;
	ECharacterType CharacterType;
	float X;
	float Y;
	float Z;
	float Yaw;
	int MaxHp;
	int Hp;
	int Damage;
	float Speed;
	uint32_t State;

	void SetLocation(float X_, float Y_, float Z_)
	{
		X = X_;
		Y = Y_;
		Z = Z_;
	};

	void AddState(ECharacterStateType NewState) { State |= NewState; }
	void RemoveState(ECharacterStateType RemoveState) { State &= ~RemoveState; }
	bool HasState(ECharacterStateType CheckState) const { return (State & CheckState) != 0; }
};

struct FAttackData
{
	FInfoData Attacker;
	FInfoData Attacked;
};

#pragma pack(push, 1)

struct Packet
{
	struct PacketHeader
	{
		EPacketType PacketType;
		uint8_t PacketSize;

		PacketHeader(EPacketType type, uint8_t size)
			: PacketType(type), PacketSize(size) {}
	};

	PacketHeader Header;

	Packet(EPacketType type)
		: Header(type, sizeof(PacketHeader)) {}
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
using AttackPacket = TPacket<FAttackData>;
using MonsterInfoPacket = TPacket<FInfoData>;
#pragma pack(pop)