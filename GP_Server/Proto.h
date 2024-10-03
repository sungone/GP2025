#pragma once
constexpr short SERVER_PORT = 4000;
constexpr int BUFSIZE = 1024;
constexpr int MAX_CLIENT = 1000;

using int32 = int;   // int32¢¬? int¡¤? ?¢´??

enum class EPacketType : uint16_t
{
	C_LOGIN,
	C_LOGINOUT,
	C_MOVE,

	S_LOGININFO
};

#pragma pack(push, 1)

struct FPacketHeader
{
	EPacketType PacketType;
	uint16_t PacketSize;
};

struct FLoginPacket
{
	FPacketHeader Header;
	int32 PlayerID;
};

struct FLoginInfoPacket
{
	FPacketHeader Header;
	int32 PlayerID;
	float X;
	float Y;
	float Z;
	float Yaw;
	float Pitch;
	float Roll;
};

struct FMovePacket
{
	FPacketHeader Header;
	int32 PlayerID;
	float X;
	float Y;
	float Z;
	float Yaw;
	float Pitch;
	float Roll;
};

#pragma pack(pop)
