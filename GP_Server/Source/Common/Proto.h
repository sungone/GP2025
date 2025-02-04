#pragma once

using BYTE = unsigned char;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

constexpr uint32 MAX_CLIENT = 1000;
constexpr BYTE PKT_TYPE_INDEX = 0;
constexpr BYTE PKT_SIZE_INDEX = 1;
constexpr int16 SERVER_PORT = 4000;
constexpr uint32 BUFSIZE = 1024;

enum EPacketType : uint8
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

enum ECharacterType : uint8
{
	P_WARRIOR,
	P_GUNNER,

	M_MOUSE,
	M_BUBBLETEA,
	M_ENERGYDRINK,
	M_COFFEE,
};

#pragma region ObjectType

namespace CharacterType
{
	enum class ECharacterType : uint8
	{
		PLAYER,
		MONSTER,
		NPC
	};

	enum class EPlayerType : uint8
	{
		WARRIOR,
		GUNNER
	};

	enum class EMonsterType : uint8
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

	enum class ENPCType : uint8
	{
		PROFESSOR,
		GUARD,
		STUDENT
	};
}

namespace ItemType
{
	enum class EItemType : uint8
	{
		WEAPON,
		ARMOR,
		CONSUMABLE,
	};

	enum class EWeaponType : uint8
	{
		BIRD_GUN,
		PULSE_GUN,
		POSITRON,
		PRACS_WORD,
		PULSE_SWORD,
		ENERGY_SWORD,
	};

	enum class EArmorType : uint8
	{
		ALLOY_HELMET,
		ENHANCED_HELMET,
		TITANIUM_ARMOR,
		POWERED_ARMOR,

		SUIT,
		TUCLOTHES
	};

	enum class EConsumableType : uint8
	{
		HPKIT_LOW,
		HPKIT_MID,
		HPKIT_HIGH,

		COFFEE,
		DDALBA,
		MANGBA,
	};
}
#pragma endregion // 추후 이렇게 바꾸려함

enum ECharacterStateType : uint8
{
	STATE_NONE = 0, // 2^0
	STATE_IDLE = 1 << 0,  // 2^1
	STATE_RUN = 1 << 1,  // 2^2
	STATE_JUMP = 1 << 2,  // 2^3
	STATE_AUTOATTACK = 1 << 3, // 2^4
	STATE_DIE = 1 << 4,
	STATE_WALK = 1 << 5,
};

#ifdef SERVER_BUILD
struct FVector
{
	double X, Y, Z;

	FVector() : X(0), Y(0), Z(0) {}
	FVector(double x, double y, double z) : X(x), Y(y), Z(z) {}

	double DistanceSquared(const FVector& Other) const
	{
		return (X - Other.X) * (X - Other.X) +
			(Y - Other.Y) * (Y - Other.Y) +
			(Z - Other.Z) * (Z - Other.Z);
	}

	bool IsInRange(const FVector& Other, double Range) const
	{
		return DistanceSquared(Other) <= (Range * Range);
	}
};
#endif

#pragma pack(push, 1)

struct FStatData
{
	int32_t Level;
	float Exp;
	float MaxExp;
	float Hp;
	float MaxHp;
	float Damage;
	float CrtRate;
	float CrtValue;
	float Dodge;

	FStatData()
		: Level(1), Exp(0.0f), MaxExp(100.0f),
		Hp(100.0f), MaxHp(100.0f),
		Damage(10.0f), CrtRate(0.1f), CrtValue(1.5f),
		Dodge(0.1f)
	{
	}
#ifdef SERVER_BUILD
	void SetHp(float NewHp) { Hp = std::clamp(NewHp, 0.0f, MaxHp); }
	void SetMaxHp(float NewMaxHp) { MaxHp = std::max(0.0f, NewMaxHp); }
	void TakeDamage(float Amount) { SetHp(Hp - Amount); }
	void Heal(float Amount) { SetHp(Hp + Amount); }
	bool IsDead() const { return Hp <= 0; }

	void SetLevel(int32_t NewLevel) { Level = std::max(1, NewLevel); }
	void SetExp(float NewExp) { Exp = std::clamp(NewExp, 0.0f, MaxExp); }
#endif
};

struct FInfoData
{
	int32 ID;
	ECharacterType CharacterType;
	FVector Pos;
	float Yaw;
	float CollisionRadius;
	float AttackRange;
	float Speed;
	FStatData Stats;
	uint32 State;

	void InitStats(float MaxHp, float Damage, float CrtRate, float CrtValue, float Dodge, float Speed_)
	{
		Stats.MaxHp = MaxHp;
		Stats.Hp = MaxHp;
		Stats.Damage = Damage;
		Stats.CrtRate = CrtRate;
		Stats.CrtValue = CrtValue;
		Stats.Dodge = Dodge;
		Speed = Speed_;
		State = STATE_IDLE;
	}

	void SetLocation(float X_, float Y_, float Z_) { Pos = FVector(X_, Y_, Z_); }
	void AddState(ECharacterStateType NewState) { State |= NewState; }
	void RemoveState(ECharacterStateType RemoveState) { State &= ~RemoveState; }
	bool HasState(ECharacterStateType CheckState) const { return (State & CheckState) != 0; }

	float GetHp() const { return Stats.Hp; }
	float GetMaxHp() const { return Stats.MaxHp; }
	float GetDamage() const { return Stats.Damage; }
	float GetCrtRate() const { return Stats.CrtRate; }
	float GetCrtValue() const { return Stats.CrtValue; }
	float GetDodge() const { return Stats.Dodge; }

#ifdef SERVER_BUILD
	void SetHp(float NewHp) { Stats.SetHp(NewHp); }
	void Heal(float Amount) { Stats.Heal(Amount); }
	void TakeDamage(float Damage) { Stats.TakeDamage(Damage); }
	void SetDamage(float NewDamage) { Stats.Damage = std::max(0.0f, NewDamage); }
	bool IsDead() const { return Stats.IsDead(); }
	bool IsInAttackRange(const FInfoData& Target) const
	{
		return Pos.IsInRange(Target.Pos, AttackRange + Target.CollisionRadius);
	}
#endif

};


struct FAttackData
{
	FInfoData Attacker;
	FInfoData Attacked;
	float AttackerDamage;
};

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
using AttackPacket = TPacket<FAttackData>;
using MonsterInfoPacket = TPacket<FInfoData>;
#pragma pack(pop)