#pragma once
#include "Common.h"

enum class DBResultCode : int32
{
	SUCCESS = 0,
	INVALID_USER = -1,
	INVALID_PASSWORD = -2,
	DUPLICATE_ID = -3,
	DB_ERROR = -99
};

#pragma pack(push,1)
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
	float Speed;

	FStatData()
		: Level(1), Exp(0.0f), MaxExp(100.0f),
		Hp(100.0f), MaxHp(100.0f),
		Damage(10.0f), CrtRate(0.5f), CrtValue(1.5f),
		Dodge(0.1f), Speed(0.0f)
	{
	}
};

struct FInfoData
{
	int32 ID;
	char NickName[NICKNAME_LEN];
	uint8 CharacterType;
	FVector Pos;
	float Yaw;
	float CollisionRadius;
	float AttackRadius;

	FStatData Stats;
	uint32 State;
	uint32 Gold;

	FInfoData()
		: ID(0), CharacterType(), NickName("None"),
		Pos(FVector(0.0f, 0.0f, 0.0f)), Yaw(0.0f),
		CollisionRadius(0.0f), AttackRadius(0.f),
		Stats(), State(STATE_IDLE)
	{
	}
	void SetYaw(float Yaw_) { Yaw = Yaw_; }
	void SetLocation(float X_, float Y_, float Z_) { Pos = FVector(X_, Y_, Z_); }
	void SetLocation(FVector Pos_) { Pos = Pos_; }
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
	float GetSpeed() const { return Stats.Speed; }
	const char* GetName() const { return NickName; }

#ifdef SERVER_BUILD
	void SetHp(float NewHp) { Stats.Hp = std::clamp(NewHp, 0.0f, Stats.MaxHp); }
	void Heal(float Amount) { SetHp(Stats.Hp + Amount); }
	void TakeDamage(float Amount) { SetHp(Stats.Hp - Amount); }
	bool IsDead() const { return Stats.Hp < 1.0f; }
	void SetDamage(float NewDamage) { Stats.Damage = std::max(0.0f, NewDamage); }
	float GetAttackDamage(float RandomValue) const
	{
		if (RandomValue < 0.1f)
			return 0.0f;

		bool bIsCritical = RandomValue < Stats.CrtRate;
		return bIsCritical ? Stats.Damage * Stats.CrtValue : Stats.Damage;
	}
	void SetLocationAndYaw(FVector NewPos)
	{
		Yaw = CalculateYaw(NewPos);
		Pos = NewPos;
	}
	float CalculateYaw(FVector TargetPos) const
	{
		return Pos.GetYawToTarget(TargetPos);
	}
	FVector GetFrontVector() const
	{
		float RadianYaw = Yaw * (3.14159265359f / 180.0f);
		float ForwardX = std::cos(RadianYaw);
		float ForwardY = std::sin(RadianYaw);
		return FVector(ForwardX, ForwardY, 0.0f).Normalize();
	}
	void SetName(const char* InNick)
	{
		strncpy_s(NickName, InNick, NICKNAME_LEN - 1);
		NickName[NICKNAME_LEN - 1] = '\0';
	}
#endif
};

#pragma pack(pop)