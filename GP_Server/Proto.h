#pragma once
constexpr short SERVER_PORT = 4000;
constexpr int BUFSIZE = 1024;
constexpr int MAX_CLIENT = 1000;

using int32 = int;   // int32�� int�� ����

enum EPacketType : uint8_t
{
	C_LOGIN,
	C_LOGOUT,
	C_MOVE,
    C_ATTACK,
    C_HIT,

	S_LOGININFO,
	S_ADD_PLAYER,
	S_REMOVE_PLAYER,
	S_MOVE_PLAYER,
    S_ATTACK_PLAYER,

    S_SPAWN_MONSTER,
};

enum ECharacterType : uint8_t
{
    // �÷��̾�
    Warrior ,
    Gunner ,

    // ����
    Mouse ,
};

enum EMoveStateType : uint32_t
{
    STATE_NONE = 0, // 2^0
    STATE_IDLE = 1 << 0,  // 2^1
    STATE_RUN = 1 << 1,  // 2^2
    STATE_JUMP = 1 << 2,  // 2^3
    STATE_AUTOATTACK = 1 << 3, // 2^4
    // �߰� ���¸� ��Ʈ �÷��׷� ��� ������ �� �ֽ��ϴ�.
};

struct FCharacterInfo
{
    int32 ID;
    float X;
    float Y;
    float Z;
    float Yaw;
    float MaxHp;
    float Hp;
    float Speed;
    uint32_t State;  // ��Ʈ �÷��׷� ����� �� �ֵ��� uint32_t�� ����

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
    FCharacterInfo PlayerInfo;
};

struct FMovePacket
{
	FPacketHeader Header;
    FCharacterInfo PlayerInfo;
};

struct FAttackPacket
{
    FPacketHeader Header;
    FCharacterInfo PlayerInfo;
};

struct FHitPacket
{
    FPacketHeader Header;
    bool isPlayer;
    float DamageAmount;
};

struct FAddPlayerPacket
{
	FPacketHeader Header;
	int32 PlayerID;
    FCharacterInfo PlayerInfo;
};

struct FRemovePlayerPacket
{
	FPacketHeader Header;
	int32 PlayerID;
};

struct FSpawnMonsterPacket
{
    FPacketHeader Header;
    FCharacterInfo MonsterInfo;
};



#pragma pack(pop)