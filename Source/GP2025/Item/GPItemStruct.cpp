// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/GPItemStruct.h"

FGPItemStruct::FGPItemStruct()
	:
	ItemName(FText::FromString(TEXT("Unknown"))) // �⺻ ������ �̸�
	, Category(ECategory::None) // �⺻ ī�װ� (None)
	, Damage(0.f) // �⺻ ���ݷ� 0
	, Hp(0.f) // �⺻ ü�� 0
	, CrtRate(0.f) // �⺻ ġ��Ÿ Ȯ�� 0%
	, DodgeRate(0.f) // �⺻ ȸ���� 0%
	, MoveSpeed(0.f) // �⺻ �̵� �ӵ� 0
	, AType(EAbilityType::None) // �⺻ �ɷ� Ÿ�� ����
	, AValue(0.f) // �ɷ� �� �⺻ 0
	, Grade(1) // �⺻ ��� 1
	, bIsMonster(false) // �⺻������ ���� ��� X
	, ResellPrice(0) // �⺻ ���ȱ� ���� 0
	, Price(0) // �⺻ ���� �Ǹ� ���� 0
	, bIsSellable(false) // �⺻������ ���� �� ����
	, bIsBuy(false) // �⺻������ �������� ���� �Ұ�
	, ItemThumbnail(nullptr) // �⺻ ����� ����
	, ItemStaticMesh(nullptr) // �⺻ StaticMesh ����
	,ItemSkeletalMesh_Man(nullptr)
	, ItemSkeletalMesh_Woman(nullptr)
	, WeaponClass(nullptr)
{

}
