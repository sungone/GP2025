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

	float Hp; // �ִ� ü��
	float MaxHp; // ���� ü��
	float atk; // ���ݷ�
	float crt_rate; // ũ��Ƽ�� Ȯ��
	float MoveSpeed; // �̵� �ӵ�
	float dodge; // ȸ�� Ȯ��
	float cooltime; // ��ų ��Ÿ��
	bool is_boss; // �Ϲݸ� or ������
	float AttackRange; // ���� ����
	float AttackSpeed; // ���� �ӵ�

	uint32_t State; // ������ �ൿ ����

private :
	// ���� �÷���
	bool isAlive;       // ���� ����
	bool isAggroed;     // ���� ���� ����
	bool isMoving;      // �̵� ����
	bool isAttacking;   // ���� ����

private :
	// �ִϸ��̼� �� ����
	std::string animationState; // ���� �ִϸ��̼� ���� (Idle, Walk, Attack ��)
	std::string behaviorTree;   // �����̺�� Ʈ�� ���� ���

private :
	// Ÿ�̸� �� ������Ʈ
	float timeSinceLastAction; // ������ �ൿ ���� ��� �ð�
	float actionInterval;      // �ൿ ������Ʈ �ֱ�
};

