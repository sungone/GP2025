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

	S_DAMAGED_MONSTER
};

//enum ECharacterType : uint8
//{
//	NONE,
//	P_WARRIOR,
//	P_GUNNER,
//
//	M_MOUSE,
//	M_BUBBLETEA,
//	M_ENERGYDRINK,
//	M_COFFEE,
//};

#pragma region ObjectType
using ECharacterType = uint8;

namespace Type
{
	enum EPlayer : uint8
	{
		WARRIOR,
		GUNNER,

		PEND
	};

	enum EMonster : uint8
	{
		ENERGY_DRINK = PEND,
		BUBBLE_TEA,
		COFFEE,

		MOUSE,
		KEYBOARD,
		DESKTOP,

		COGWHEEL,
		BOLT_NUT,
		DRILL,

		TINO,
	};

	enum ENpc : uint8
	{
		PROFESSOR,
		GUARD,
		STUDENT,
	};

	enum class EWeapon : uint8
	{
		BIRD_GUN,
		PULSE_GUN,
		POSITRON,
		PRACS_WORD,
		PULSE_SWORD,
		ENERGY_SWORD,
	};

	enum class EArmor : uint8
	{
		ALLOY_HELMET,
		ENHANCED_HELMET,
		TITANIUM_ARMOR,
		POWERED_ARMOR,

		SUIT,
		TUCLOTHES
	};

	enum class EConsumable : uint8
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

#pragma pack(push, 1)

struct FStatData
{
	uint32 Level;
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
};

struct FInfoData
{
	int32 ID;
	uint8 CharacterType;
	FVector Pos;
	float Yaw;
	float CollisionRadius;

	float AttackRadius;
	float AttackRange;

	float Speed;
	FStatData Stats;
	uint32 State;

	FInfoData()
		: ID(0), CharacterType(),
		Pos(FVector(0.0f, 0.0f, 0.0f)), Yaw(0.0f),
		CollisionRadius(100.0f), AttackRadius(200.0f),
		AttackRange(200.0f), Speed(0.0f),
		Stats(), State(STATE_IDLE)
	{
	}

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

	int32 GetLevel() const { return Stats.Level; }
	float GetHp() const { return Stats.Hp; }
	float GetMaxHp() const { return Stats.MaxHp; }
	float GetExp() const { return Stats.Exp; }
	float GetMaxExp() const { return Stats.MaxExp; }
	float GetDamage() const { return Stats.Damage; }
	float GetCrtRate() const { return Stats.CrtRate; }
	float GetCrtValue() const { return Stats.CrtValue; }
	float GetDodge() const { return Stats.Dodge; }
	FVector GetAttackPos() const
	{
		return Pos + FVector(AttackRange * 0.5f, 0.0f, 0.0f);
	}
#ifdef SERVER_BUILD
	void SetHp(float NewHp) { Stats.Hp = std::clamp(NewHp, 0.0f, Stats.MaxHp); }
	void Heal(float Amount) { SetHp(Stats.Hp + Amount); }
	void TakeDamage(float Amount) { SetHp(Stats.Hp - Amount); }
	bool IsDead() const { return Stats.Hp <= 0; }
	void SetDamage(float NewDamage) { Stats.Damage = std::max(0.0f, NewDamage); }

	bool IsInAttackRange(const FInfoData& Target) const
	{
		return Pos.IsInRange(Target.Pos, AttackRange + Target.CollisionRadius);
	}
	float GetAttackDamage() const
	{
		static std::default_random_engine dre;
		static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		float RandomValue = dist(dre);
		if (RandomValue < 0.1f)
			return 0.0f;

		bool bIsCritical = RandomValue < Stats.CrtRate;
		return bIsCritical ? Stats.Damage * Stats.CrtValue : Stats.Damage;
	}
#endif
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
#pragma pack(pop)