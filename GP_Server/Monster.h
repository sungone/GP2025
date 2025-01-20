#pragma once
#include <string>
#include "Proto.h"

class Monster
{
public :
	Monster(int32 ID, ECharacterType MonsterType , float LocationX, float LocationY, float LocationZ, float Yaw);

	FCharacterInfo GetInfo() const;
	int32 GetID() const { return ID; }
	float GetHp() const { return Hp; }

	void Attacked(float damage);

protected :
	// Monster Info
	int32 ID;
	ECharacterType MonsterType;

	// Monster Location ( World Space )
	float LocationX;
	float LocationY;
	float LocationZ;

	// Monster Rotation ( World Space ) 
	float RotationRoll;
	float RotationPitch;
	float RotationYaw;

	// Monster Scale ( World Space )
	float ScaleX;
	float ScaleY;
	float ScaleZ;

	float Hp; // 현재 체력
	float MaxHp; // 최대 체력
	float Attack; // 공격력
	float CrtRate; // 크리티컬 확률
	float MoveSpeed; // 이동 속도
	float Dodge; // 회피 확률
	float Cooltime; // 스킬 쿨타임
	bool isBoss; // 일반몹 or 보스몹
	float AttackRange; // 공격 범위
	float AttackSpeed; // 공격 속도

	uint32_t State; // 몬스터의 행동 상태

//private :
//	// 애니메이션 및 동작
//	std::string animationState; // 현재 애니메이션 상태 (Idle, Walk, Attack 등)
//	std::string behaviorTree;   // 비헤이비어 트리 파일 경로
//
//private :
//	// 타이머 및 업데이트
//	float timeSinceLastAction; // 마지막 행동 이후 경과 시간
//	float actionInterval;      // 행동 업데이트 주기
};

