constexpr short SERVER_PORT = 4000;
constexpr int BUFSIZE = 1024;
constexpr int MAX_CLIENT = 1000;

using int32 = int;   // int32를 int로 정의

enum class EPacketType : uint8_t
{
	C_LOGIN,
	C_LOGINOUT,
	C_MOVE,

	S_LOGININFO,
	S_ADD_PLAYER,
	S_REMOVE_PLAYER,
	S_MOVE_PLAYER,
};

struct FVectorInfo
{
	float X;
	float Y;
	float Z;
	float Yaw;
	float Pitch;
	float Roll;
};

#pragma pack(push, 1)

struct FPacketHeader
{
	uint8_t PacketSize;
	EPacketType PacketType;
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
	FVectorInfo VecInfo;
};

struct FMovePacket
{
	FPacketHeader Header;
	int32 PlayerID;
	FVectorInfo VecInfo;
};

struct FAddPlayerPacket
{
	FPacketHeader Header;
	int32 PlayerID;
	FVectorInfo VecInfo;
};

#pragma pack(pop)