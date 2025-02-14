#pragma once
#include "Common.h"

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
	FVector GetAttackPos() const
	{
		return Pos + FVector(AttackRange * 0.5f, 0.0f, 0.0f);
	}
#ifdef SERVER_BUILD
	void SetHp(float NewHp) { Stats.Hp = std::clamp(NewHp, 0.0f, Stats.MaxHp); }
	void Heal(float Amount) { SetHp(Stats.Hp + Amount); }
	void TakeDamage(float Amount) { SetHp(Stats.Hp - Amount); }
	bool IsDead() const { return Stats.Hp < 1.0f; }
	void SetDamage(float NewDamage) { Stats.Damage = std::max(0.0f, NewDamage); }

	bool IsInAttackRange(const FInfoData& Target) const
	{
		return Pos.IsInRange(Target.Pos, AttackRange + Target.CollisionRadius);
	}
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
		FVector Direction = (TargetPos - Pos).Normalize();
		return std::atan2(Direction.Y, Direction.X) * (180.0f / 3.14159265f);
	}

#endif
};