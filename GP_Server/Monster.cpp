#include "Monster.h"

Monster::Monster(int32 ID, EMonster MonsterType , float LocationX , float LocationY , float LocationZ , float Yaw)
	: ID(ID) , MonsterType(MonsterType) , LocationX(LocationX) , LocationY(LocationY) 
    , LocationZ(LocationZ) , RotationYaw(Yaw)
{
    // �⺻ �Ӽ� �ʱ�ȭ
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

    // ���� ID
    Info.ID = ID;

    // ��ġ �� ȸ��
    Info.X = LocationX;
    Info.Y = LocationY;
    Info.Z = LocationZ;
    Info.Yaw = RotationYaw;

    // ü��
    Info.Hp = Hp;
    Info.MaxHp = MaxHp;

    // ����
    Info.State = State;

    return Info;
}
