// Fill out your copyright notice in the Description p age of Project Settings.


#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterControlData.h"
#include "Item/GPItemStruct.h"
#include "Weapons/GPWeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Character/Modules/GPPlayerEffectHandler.h"
#include "GPCharacterPlayer.h"


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

    SetCharacterType(CurrentCharacterType);
}

void AGPCharacterPlayer::BeginPlay()
{
    Super::BeginPlay();
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
            AppearanceHandler->AddToRoot();
        }
    }

    if (!EffectHandler)
    {
        EffectHandler = NewObject<UGPPlayerEffectHandler>(this);
        if (EffectHandler)
        {
            EffectHandler->Init(this);
            EffectHandler->AddToRoot();
        }
    }

    UGPCharacterControlData** FoundData = CharacterTypeManager.Find(CurrentCharacterType);
    if (FoundData && *FoundData && AppearanceHandler)
    {
        AppearanceHandler->ApplyCharacterPartsFromData(*FoundData);
    }
}

void AGPCharacterPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (AppearanceHandler)
    {
        if (AppearanceHandler->IsRooted())
        {
            AppearanceHandler->RemoveFromRoot();
            UE_LOG(LogTemp, Warning, TEXT("AppearanceHandler RemoveFromRoot() called in EndPlay."));
        }
        AppearanceHandler = nullptr;
    }

    if (EffectHandler)
    {
        if (EffectHandler->IsRooted())
        {
            EffectHandler->RemoveFromRoot();
        }
        EffectHandler = nullptr;
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

bool AGPCharacterPlayer::bIsGunnerCharacter() const
{
    return (CurrentCharacterType == (uint8)Type::EPlayer::GUNNER);
}

USkeletalMeshComponent* AGPCharacterPlayer::GetCharacterMesh() const
{
    return BodyMesh;
}