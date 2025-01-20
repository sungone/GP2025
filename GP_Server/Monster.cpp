#include "Monster.h"

Monster::Monster(int32 ID, ECharacterType MonsterType , float LocationX , float LocationY , float LocationZ , float Yaw)
	: ID(ID) , MonsterType(MonsterType) , LocationX(LocationX) , LocationY(LocationY) , LocationZ(LocationZ) , RotationYaw(Yaw)
{
    // 기본 속성 초기화
    Hp = 100.f;
    MaxHp = 100.f;
    Attack = 20.f;
    CrtRate = 0.1f;
    MoveSpeed = 300.f;
    Dodge = 0.05f;
    Cooltime = 2.0f;
    AttackRange = 150.f;
    AttackSpeed = 1.0f;
    isBoss = false;
    State = 0;
}

FCharacterInfo Monster::GetInfo() const
{
    FCharacterInfo Info;

    // 고유 ID
    Info.ID = ID;

    // 몬스터 종류
    Info.CharacterType = MonsterType;

    // 위치 및 회전
    Info.X = LocationX;
    Info.Y = LocationY;
    Info.Z = LocationZ;
    Info.Yaw = RotationYaw;

    // 체력
    Info.Hp = Hp;
    Info.MaxHp = MaxHp;

    // 공격력
    Info.Damage = Attack;

    // 상태
    Info.State = State;

    return Info;
}

void Monster::Attacked(float damage)
{
    if ((Hp - damage) <= 0) 
        Hp = 0;
    else
    {
        Hp = Hp - damage;
    }
}
