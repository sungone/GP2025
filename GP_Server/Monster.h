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

	float Hp; // ���� ü��
	float MaxHp; // �ִ� ü��
	float Attack; // ���ݷ�
	float CrtRate; // ũ��Ƽ�� Ȯ��
	float MoveSpeed; // �̵� �ӵ�
	float Dodge; // ȸ�� Ȯ��
	float Cooltime; // ��ų ��Ÿ��
	bool isBoss; // �Ϲݸ� or ������
	float AttackRange; // ���� ����
	float AttackSpeed; // ���� �ӵ�

	uint32_t State; // ������ �ൿ ����

//private :
//	// �ִϸ��̼� �� ����
//	std::string animationState; // ���� �ִϸ��̼� ���� (Idle, Walk, Attack ��)
//	std::string behaviorTree;   // �����̺�� Ʈ�� ���� ���
//
//private :
//	// Ÿ�̸� �� ������Ʈ
//	float timeSinceLastAction; // ������ �ൿ ���� ��� �ð�
//	float actionInterval;      // �ൿ ������Ʈ �ֱ�
};

