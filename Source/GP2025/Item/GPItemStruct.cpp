// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/GPItemStruct.h"

FGPItemStruct::FGPItemStruct()
	:
	ItemName(FText::FromString(TEXT("Unknown"))) // 기본 아이템 이름
	, Category(ECategory::None) // 기본 카테고리 (None)
	, Damage(0.f) // 기본 공격력 0
	, Hp(0.f) // 기본 체력 0
	, CrtRate(0.f) // 기본 치명타 확률 0%
	, DodgeRate(0.f) // 기본 회피율 0%
	, MoveSpeed(0.f) // 기본 이동 속도 0
	, AType(EAbilityType::None) // 기본 능력 타입 없음
	, AValue(0.f) // 능력 값 기본 0
	, Grade(1) // 기본 등급 1
	, bIsMonster(false) // 기본적으로 몬스터 드랍 X
	, ResellPrice(0) // 기본 되팔기 가격 0
	, Price(0) // 기본 상점 판매 가격 0
	, bIsSellable(false) // 기본적으로 되팔 수 없음
	, bIsBuy(false) // 기본적으로 상점에서 구매 불가
	, ItemThumbnail(nullptr) // 기본 썸네일 없음
	, ItemStaticMesh(nullptr) // 기본 StaticMesh 없음
	,ItemSkeletalMesh_Man(nullptr)
	, ItemSkeletalMesh_Woman(nullptr)
	, WeaponClass(nullptr)
{

}
