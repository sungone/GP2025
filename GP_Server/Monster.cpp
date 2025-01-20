#include "Monster.h"

Monster::Monster(int32 ID, ECharacterType MonsterType , float LocationX , float LocationY , float LocationZ , float Yaw)
	: ID(ID) , MonsterType(MonsterType) , LocationX(LocationX) , LocationY(LocationY) , LocationZ(LocationZ) , RotationYaw(Yaw)
{
    // �⺻ �Ӽ� �ʱ�ȭ
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

    // ���� ID
    Info.ID = ID;

    // ���� ����
    Info.CharacterType = MonsterType;

    // ��ġ �� ȸ��
    Info.X = LocationX;
    Info.Y = LocationY;
    Info.Z = LocationZ;
    Info.Yaw = RotationYaw;

    // ü��
    Info.Hp = Hp;
    Info.MaxHp = MaxHp;

    // ���ݷ�
    Info.Damage = Attack;

    // ����
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
