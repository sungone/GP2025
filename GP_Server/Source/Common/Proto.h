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
	S_MOVE_MONSTER,
	S_MONSTER_STATUS_UPDATE,
};

enum ECharacterType : uint8_t
{
	//INVALID = -1,

	P_WARRIOR,
	P_GUNNER,

	M_MOUSE,
};

enum EMoveStateType : uint32_t
{
	STATE_NONE = 0, // 2^0
	STATE_IDLE = 1 << 0,  // 2^1
	STATE_RUN = 1 << 1,  // 2^2
	STATE_JUMP = 1 << 2,  // 2^3
	STATE_AUTOATTACK = 1 << 3, // 2^4
};

struct FInfoData
{
	int32 ID;
	ECharacterType CharacterType;
	float X;
	float Y;
	float Z;
	float Yaw;
	float MaxHp;
	float Hp;
	float Damage;
	float Speed;
	uint32_t State;

	void SetLocation(float X_, float Y_, float Z_)
	{
		X = X_;
		Y = Y_;
		Z = Z_;
	};
	void AddState(EMoveStateType NewState) { State |= NewState; }
	void RemoveState(EMoveStateType RemoveState) { State &= ~RemoveState; }
	bool HasState(EMoveStateType CheckState) const { return (State & CheckState) != 0; }
};

struct FAttackData
{
	int32 AttackerID;
	int32 AttackedID;
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
#pragma pack(pop)