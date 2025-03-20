// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterMonster.h"
#include "Character/GPCharacterControlData.h"

AGPCharacterMonster::AGPCharacterMonster()
{
	HpBar = CreateWidgetComponent(TEXT("HpWidget"), TEXT("/Game/UI/WBP_CharacterHpBar.WBP_CharacterHpBar_C"), FVector(0.f, 0.f, 300.f), FVector2D(150.f, 15.f));
	LevelText = CreateWidgetComponent(TEXT("LevelWidget"), TEXT("/Game/UI/WBP_LevelText.WBP_LevelText_C"), FVector(0.f, 0.f, 330.f), FVector2D(40.f, 10.f));
}

void AGPCharacterMonster::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterType(CurrentCharacterType);
}

void AGPCharacterMonster::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
	Super::SetCharacterData(CharacterControlData);
	GetMesh()->SetSkeletalMesh(CharacterControlData->SkeletalMesh);
	GetMesh()->SetAnimInstanceClass(CharacterControlData->AnimBlueprint);
}

void AGPCharacterMonster::SetCharacterType(ECharacterType NewCharacterControlType)
{
	Super::SetCharacterType(NewCharacterControlType);
}
