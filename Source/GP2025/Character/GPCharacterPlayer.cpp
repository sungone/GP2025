// Fill out your copyright notice in the Description p age of Project Settings.


#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterControlData.h"
#include "Item/GPItemStruct.h"
#include "Weapons/GPWeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"


AGPCharacterPlayer::AGPCharacterPlayer()
{
    Helmet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet"));
    Helmet->SetupAttachment(GetMesh(), TEXT("HelmetSocket"));
    Helmet->SetCollisionProfileName(TEXT("NoCollision"));
    Helmet->SetVisibility(true);

    BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(GetMesh());
    BodyMesh->SetCollisionProfileName(TEXT("NoCollision"));

    HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetupAttachment(BodyMesh);

    LegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegMesh"));
    LegMesh->SetupAttachment(BodyMesh);
}

void AGPCharacterPlayer::BeginPlay()
{
    Super::BeginPlay();
    SetCharacterType(CurrentCharacterType);
}

void AGPCharacterPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AGPCharacterPlayer::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (!AppearanceHandler)
    {
        AppearanceHandler = NewObject<UGPPlayerAppearanceHandler>(this, UGPPlayerAppearanceHandler::StaticClass());
        if (AppearanceHandler)
        {
            AppearanceHandler->Initialize(this);
        }
    }

    UGPCharacterControlData** FoundData = CharacterTypeManager.Find(CurrentCharacterType);
    if (FoundData && *FoundData && AppearanceHandler)
    {
        AppearanceHandler->ApplyCharacterPartsFromData(*FoundData);
    }
}

void AGPCharacterPlayer::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
    Super::SetCharacterData(CharacterControlData);

    if (CharacterControlData && BodyMesh)
    {
        BodyMesh->SetSkeletalMesh(CharacterControlData->BodyMesh);
        if (CharacterControlData->AnimBlueprint)
        {
            BodyMesh->SetAnimInstanceClass(CharacterControlData->AnimBlueprint);
        }
    }
}

void AGPCharacterPlayer::SetCharacterType(ECharacterType NewCharacterControlType)
{
    Super::SetCharacterType(NewCharacterControlType);

    UGPCharacterControlData* NewCharacterData = CharacterTypeManager[NewCharacterControlType];
    check(NewCharacterData);

    SetCharacterData(NewCharacterData);

    CurrentCharacterType = NewCharacterControlType;

    if (AppearanceHandler)
    {
        AppearanceHandler->ApplyCharacterPartsFromData(NewCharacterData);
    }
}

USkeletalMeshComponent* AGPCharacterPlayer::GetCharacterMesh() const
{
    return BodyMesh;
}