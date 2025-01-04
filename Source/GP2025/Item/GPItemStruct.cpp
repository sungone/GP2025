// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/GPItemStruct.h"

FGPItemStruct::FGPItemStruct()
	: ItemIndex(0) , ItemName (TEXT("Default")) , ItemMesh(nullptr) , Amount(0) , Category(EItemCategoryType::NONE)
	, DetailType(EItemDetailType::NONE) , Attack(0.f) , Hp(0.f) , CriticalRate(0.f) 
	, DodgeRate(0.f) , MoveSpeed(0.f) , AbilityType(EItemAbilityType::NONE) , AbilityValue(0.f)
	, Grade(0) , bIsMonster(false) , bCanResell(false) , bIsBuy(false) , Price(0)
{

}
