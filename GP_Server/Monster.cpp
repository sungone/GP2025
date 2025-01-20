#include "Monster.h"

Monster::Monster(int32 ID, EMonster MonsterType , float LocationX , float LocationY , float LocationZ , float Yaw)
	: ID(ID) , MonsterType(MonsterType) , LocationX(LocationX) , LocationY(LocationY) 
    , LocationZ(LocationZ) , RotationYaw(Yaw)
{
    // 기본 속성 초기화
    Hp = 100.f;
    MaxHp = 100.f;
    atk = 20.f;
    crt_rate = 0.1f;
    MoveSpeed = 300.f;
    dodge = 0.05f;
    cooltime = 2.0f;
    AttackRange = 150.f;
    AttackSpeed = 1.0f;

    is_boss = false;

    State = 0;

    isAlive = true;
    isAggroed = false;
    isMoving = false;
    isAttacking = false;
}

FCharacterInfo Monster::GetInfo() const
{
    FCharacterInfo Info;

    // 고유 ID
    Info.ID = ID;

    // 위치 및 회전
    Info.X = LocationX;
    Info.Y = LocationY;
    Info.Z = LocationZ;
    Info.Yaw = RotationYaw;

    // 체력
    Info.Hp = Hp;
    Info.MaxHp = MaxHp;

    // 상태
    Info.State = State;

    return Info;
}
