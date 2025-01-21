#pragma once
constexpr short SERVER_PORT = 4000;
constexpr int BUFSIZE = 1024;
constexpr int MAX_CLIENT = 1000;

using int32 = int;   // int32를 int로 정의

enum EPacketType : uint8_t
{
	C_LOGIN,
	C_LOGOUT,
	C_MOVE,
	C_ATTACK,

	S_LOGININFO,
	S_ADD_PLAYER,
	S_REMOVE_PLAYER,
	S_MOVE_PLAYER,
	S_ATTACK_PLAYER,
};

enum EMoveStateType : uint32_t
{
	STATE_NONE = 0, // 2^0
	STATE_IDLE = 1 << 0,  // 2^1
	STATE_RUN = 1 << 1,  // 2^2
	STATE_JUMP = 1 << 2,  // 2^3
	STATE_AUTOATTACK = 1 << 3, // 2^4
	// 추가 상태를 비트 플래그로 계속 정의할 수 있습니다.
};

struct FPlayerInfo
{
	int32 ID;
	float X;
	float Y;
	float Z;
	float Yaw;
	float Speed;
	uint32_t State;  // 비트 플래그로 사용할 수 있도록 uint32_t로 변경

	void SetLocation(float X_, float Y_, float Z_)
	{
		X = X_;
		Y = Y_;
		Z = Z_;
	};

	void AddState(EMoveStateType NewState)
	{
		State |= NewState;
	}

	void RemoveState(EMoveStateType RemoveState)
	{
		State &= ~RemoveState;
	}

	bool HasState(EMoveStateType CheckState) const
	{
		return (State & CheckState) != 0;
	}
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
		: Packet(type) , Data(payload)
	{
		Header.PacketSize = sizeof(PacketHeader) + sizeof(T);
	}
};

using InfoPacket = TPacket<FPlayerInfo>;
using IDPacket = TPacket<int32>;
using FPacketHeader = Packet::PacketHeader;
#pragma pack(pop)