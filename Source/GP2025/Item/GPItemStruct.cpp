// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/GPItemStruct.h"

FGPItemStruct::FGPItemStruct()
	: ItemIndex(0) ,  Category(EItemCategoryType::NONE)
	, DetailType(EItemDetailType::NONE) , Attack(0.f) , Hp(0.f) , CriticalRate(0.f) 
	, DodgeRate(0.f) , MoveSpeed(0.f) , AbilityType(EItemAbilityType::NONE) , AbilityValue(0.f)
	, Grade(0) , bIsMonster(false) , ResellPrice(0) , Price(0) , bCanResell(false) , bIsBuy(false),
	ItemStackSize(0), ItemThumbnail(nullptr), ItemMesh(nullptr)
{

}
