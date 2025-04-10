// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterMonster.h"
#include "Character/GPCharacterMyPlayer.h"
#include "Character/GPCharacterControlData.h"
#include "Character/Modules/GPCharacterUIHandler.h"
#include "UI/GPHpBarWidget.h"
#include "UI/GPLevelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

AGPCharacterMonster::AGPCharacterMonster()
{
    GetMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AGPCharacterMonster::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterType(CurrentCharacterType);
}

void AGPCharacterMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UIHandler->UpdateAllWidgetVisibility();
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
