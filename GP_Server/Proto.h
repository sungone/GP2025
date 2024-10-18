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

	S_LOGININFO,
	S_ADD_PLAYER,
	S_REMOVE_PLAYER,
	S_MOVE_PLAYER,
};

enum EMoveStateType : uint8_t
{
	STATE_IDLE,
	STATE_WALK,
	STATE_RUN,
	STATE_JUMP
};

struct FPlayerInfo
{
	int32 ID;
	float X;
	float Y;
	float Z;
	float Yaw;
	int State;

	void SetVector(float X_, float Y_, float Z_)
	{
		X = X_;
		Y = Y_;
		Z = Z_;
	};
};

#pragma pack(push, 1)

struct FPacketHeader
{
	EPacketType PacketType;
	uint8_t PacketSize;
};

struct FLoginPacket
{
	FPacketHeader Header;
};

struct FLogoutPacket
{
	FPacketHeader Header;
	int32 PlayerID;
};

struct FLoginInfoPacket
{
	FPacketHeader Header;
	FPlayerInfo PlayerInfo;
};

struct FMovePacket
{
	FPacketHeader Header;
	FPlayerInfo PlayerInfo;
};

struct FAddPlayerPacket
{
	FPacketHeader Header;
	int32 PlayerID;
	FPlayerInfo PlayerInfo;
};

struct FRemovePlayerPacket
{
	FPacketHeader Header;
	int32 PlayerID;
};

#pragma pack(pop)