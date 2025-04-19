// Fill out your copyright notice in the Description p age of Project Settings.


#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterControlData.h"
#include "Item/GPItemStruct.h"
#include "Weapons/GPWeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"


AGPCharacterPlayer::AGPCharacterPlayer()
{
    BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(GetMesh());
    BodyMesh->SetCollisionProfileName(TEXT("NoCollision"));

    Helmet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet"));
    Helmet->SetupAttachment(BodyMesh, TEXT("HelmetSocket"));
    Helmet->SetCollisionProfileName(TEXT("NoCollision"));
    Helmet->SetVisibility(true);

    HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetupAttachment(BodyMesh);
    HeadMesh->SetRelativeLocation(FVector(0.f, 0.f, -4.0f));

    LegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegMesh"));
    LegMesh->SetupAttachment(BodyMesh);

    EquippedItemIDs.Add(ECategory::bow, -1);
    EquippedItemIDs.Add(ECategory::sword, -1);
    EquippedItemIDs.Add(ECategory::helmet, -1);
    EquippedItemIDs.Add(ECategory::chest, -1);
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