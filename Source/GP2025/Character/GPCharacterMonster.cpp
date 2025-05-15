// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterMonster.h"
#include "Character/GPCharacterMyPlayer.h"
#include "Character/GPCharacterControlData.h"
#include "Character/Modules/GPCharacterUIHandler.h"
#include "UI/GPHpBarWidget.h"
#include "UI/GPLevelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "UI/GPWidgetComponent.h"
#include "Components/SceneComponent.h"

AGPCharacterMonster::AGPCharacterMonster()
{
    GetMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AGPCharacterMonster::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterType(CurrentCharacterType);

	// 플레이어와 충돌 설정 (카메라 시야 방해 때문에 설정)
	USkeletalMeshComponent* MyMesh = GetMesh();
	MyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MyMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	MyMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	MyMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	MyMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AGPCharacterMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (UIHandler)
		UIHandler->UpdateWidgetVisibility();
}

void AGPCharacterMonster::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
	Super::SetCharacterData(CharacterControlData);
	GetMesh()->SetSkeletalMesh(CharacterControlData->SkeletalMesh);
	GetMesh()->SetAnimInstanceClass(CharacterControlData->AnimBlueprint);

	GetCapsuleComponent()->SetCapsuleHalfHeight(CharacterControlData->CapsuleHalfHeight);
	GetCapsuleComponent()->SetCapsuleRadius(CharacterControlData->CapsuleRadius);

	if (CharacterControlData->bIsBoos)
		ApplyCapsuleAndMeshScaling(CharacterControlData->CapsuleRadius, CharacterControlData->CapsuleHalfHeight);
}

void AGPCharacterMonster::SetCharacterType(ECharacterType NewCharacterControlType)
{
	Super::SetCharacterType(NewCharacterControlType);
}

void AGPCharacterMonster::ApplyCapsuleAndMeshScaling(float CapsuleRadius, float CapsuleHalfHeight)
{
	const float BaseRadius = 42.f;
	const float BaseHalfHeight = 99.f;
		
	const float RadiusScaleRatio = CapsuleRadius / BaseRadius;
	const float HeightScaleRatio = CapsuleHalfHeight / BaseHalfHeight;

	const float FinalScale = (RadiusScaleRatio + HeightScaleRatio) * 0.5f;

	GetMesh()->SetWorldScale3D(FVector(FinalScale));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleHalfHeight));
}
