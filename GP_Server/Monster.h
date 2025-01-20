#pragma once
#include <string>
#include "Proto.h"

enum EMonster : uint8_t
{
	M_EnergyDrink,
	M_BubbleTea,
	M_Coffee,
	M_Mouse,
	M_KeyBoard,
	M_Desktop,
	M_Cogwheel,
	M_BoltNut,
	M_DrillBoss,
	M_DecadenceTino
};

class Monster
{
public :
	Monster(int32 ID, EMonster MonsterType, float LocationX, float LocationY, float LocationZ, float Yaw);

	FCharacterInfo GetInfo() const;
private :
	// Monster Info
	int32 ID;
	int32 Chapter;
	EMonster MonsterType;

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

	float Hp; // 최대 체력
	float MaxHp; // 현재 체력
	float atk; // 공격력
	float crt_rate; // 크리티컬 확률
	float MoveSpeed; // 이동 속도
	float dodge; // 회피 확률
	float cooltime; // 스킬 쿨타임
	bool is_boss; // 일반몹 or 보스몹
	float AttackRange; // 공격 범위
	float AttackSpeed; // 공격 속도

	uint32_t State; // 몬스터의 행동 상태

private :
	// 상태 플래그
	bool isAlive;       // 생존 여부
	bool isAggroed;     // 전투 상태 여부
	bool isMoving;      // 이동 여부
	bool isAttacking;   // 공격 여부

private :
	// 애니메이션 및 동작
	std::string animationState; // 현재 애니메이션 상태 (Idle, Walk, Attack 등)
	std::string behaviorTree;   // 비헤이비어 트리 파일 경로

private :
	// 타이머 및 업데이트
	float timeSinceLastAction; // 마지막 행동 이후 경과 시간
	float actionInterval;      // 행동 업데이트 주기
};

