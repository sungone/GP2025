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

enum EMoveStateType : uint32_t
{
    STATE_NONE = 0, // 2^0
    STATE_IDLE = 1 << 0,  // 2^1
    STATE_RUN = 1 << 1,  // 2^2
    STATE_JUMP = 1 << 2,  // 2^3
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