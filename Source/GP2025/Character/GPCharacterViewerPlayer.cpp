// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterViewerPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Character/GPCharacterControlData.h"
#include "Network/GPGameInstance.h"
#include "Item/GPEquipItemData.h"

AGPCharacterViewerPlayer::AGPCharacterViewerPlayer()
{
	Chest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));
	Chest->SetupAttachment(GetMesh(), TEXT("ChestSocket"));
	Chest->SetCollisionProfileName(TEXT("NoCollision"));
	Chest->SetVisibility(true);

	Helmet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet"));
	Helmet->SetupAttachment(GetMesh(), TEXT("HelmetSocket"));
	Helmet->SetCollisionProfileName(TEXT("NoCollision"));
	Helmet->SetVisibility(true);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetCollisionProfileName(TEXT("NoCollision"));
	BodyMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(BodyMesh);

	LegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegMesh"));
	LegMesh->SetupAttachment(BodyMesh);
}

void AGPCharacterViewerPlayer::BeginPlay()
{
	Super::BeginPlay();

	UGPCharacterControlData* LoadedCharacterData = LoadObject<UGPCharacterControlData>(nullptr, TEXT("/Game/CharacterType/GPC_Warrior.GPC_Warrior"));
	if (LoadedCharacterData)
	{
		ApplyCharacterPartsFromData(LoadedCharacterData);
	}
}

void AGPCharacterViewerPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGPCharacterViewerPlayer::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
	Super::SetCharacterData(CharacterControlData);

	if (CharacterControlData->AnimBlueprint)
	{
		BodyMesh->SetAnimInstanceClass(CharacterControlData->AnimBlueprint);
	}
}

void AGPCharacterViewerPlayer::SetCharacterType(ECharacterType NewCharacterControlType)
{ 
	Super::SetCharacterType(NewCharacterControlType);
}


void AGPCharacterViewerPlayer::SetupMasterPose()
{
	if (BodyMesh)
	{
		if (HeadMesh)
		{
			HeadMesh->SetMasterPoseComponent(BodyMesh);
		}
		if (LegMesh)
		{
			LegMesh->SetMasterPoseComponent(BodyMesh);
		}
	}
}

void AGPCharacterViewerPlayer::ApplyCharacterPartsFromData(const UGPCharacterControlData* CharacterData)
{
	if (!CharacterData) return;

	if (CharacterData->BodyMesh)
	{
		BodyMesh->SetSkeletalMesh(CharacterData->BodyMesh);
	}

	if (CharacterData->HeadMesh)
	{
		HeadMesh->SetSkeletalMesh(CharacterData->HeadMesh);
	}

	if (CharacterData->LegMesh)
	{
		LegMesh->SetSkeletalMesh(CharacterData->LegMesh);
	}

	if (CharacterData->HelmetMesh)
	{
		Helmet->SetSkeletalMesh(CharacterData->HelmetMesh);
		Helmet->AttachToComponent(HeadMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
		Helmet->SetVisibility(true);
	}
	else
	{
		Helmet->SetSkeletalMesh(nullptr);
		Helmet->SetVisibility(false);
	}

	if (CharacterData->ChestMesh)
	{
		Chest->SetSkeletalMesh(CharacterData->ChestMesh);
		Chest->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("ChestSocket"));
		Chest->SetVisibility(true);
	}
	else
	{
		Chest->SetSkeletalMesh(nullptr);
		Chest->SetVisibility(false);
	}

	SetupMasterPose();
}

void AGPCharacterViewerPlayer::AttackHitCheck()
{
	Super::AttackHitCheck();

}

