// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Character/GPCharacterControlData.h"
#include "Network/GPGameInstance.h"
#include "Item/GPEquipItemData.h"
#include "Weapons/GPWeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

AGPCharacterPlayer::AGPCharacterPlayer()
{
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

    ExpBar = CreateWidgetComponent(TEXT("ExpWidget"), TEXT("/Game/UI/WBP_ExpBar.WBP_ExpBar_C"), FVector(0.f, 0.f, 308.f), FVector2D(150.f, 15.f));
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

    UGPCharacterControlData* LoadedCharacterData = LoadObject<UGPCharacterControlData>(nullptr, TEXT("/Game/CharacterType/GPC_Warrior.GPC_Warrior"));
    if (LoadedCharacterData)
    {
        ApplyCharacterPartsFromData(LoadedCharacterData);
    }
}

void AGPCharacterPlayer::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
    Super::SetCharacterData(CharacterControlData);

    if (CharacterControlData->AnimBlueprint)
    {
        BodyMesh->SetAnimInstanceClass(CharacterControlData->AnimBlueprint);
    }
}

void AGPCharacterPlayer::SetCharacterType(ECharacterType NewCharacterControlType)
{
    Super::SetCharacterType(NewCharacterControlType);
}

USkeletalMeshComponent* AGPCharacterPlayer::GetCharacterMesh() const
{
    return BodyMesh;
}


void AGPCharacterPlayer::SetupMasterPose()
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

void AGPCharacterPlayer::ApplyCharacterPartsFromData(const UGPCharacterControlData* CharacterData)
{
    if (!CharacterData) return;

    if (CharacterData->BodyMesh)
    {
        TSubclassOf<UAnimInstance> PrevAnimBP = BodyMesh->GetAnimClass();
        BodyMesh->SetSkeletalMesh(CharacterData->BodyMesh);
        if (PrevAnimBP)
        {
            BodyMesh->SetAnimInstanceClass(PrevAnimBP);
        }
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

    EquipWeaponFromData(CharacterData);
    SetupMasterPose();
}

void AGPCharacterPlayer::EquipWeaponFromData(const UGPCharacterControlData* CharacterData)
{
    if (!CharacterData) return;

    if (WeaponActor)
    {
        WeaponActor->Destroy();
        WeaponActor = nullptr;
    }

    if (CharacterData->WeaponClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        if (BodyMesh == nullptr)
            return;

        WeaponActor = GetWorld()->SpawnActor<AGPWeaponBase>(CharacterData->WeaponClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
        if (WeaponActor)
        {
            USkeletalMeshComponent* MeshComp = BodyMesh;

            if (MeshComp && MeshComp->DoesSocketExist(TEXT("WeaponSocket")))
            {
                WeaponActor->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
                UE_LOG(LogTemp, Warning, TEXT("Attached WeaponActor -> WeaponSocket!"));
            }
        }

        if (CharacterData->WeaponMesh)
        {
            WeaponActor->SetWeaponMesh(CharacterData->WeaponMesh);
        }
    }
}

void AGPCharacterPlayer::AttackHitCheck()
{
    Super::AttackHitCheck();
}

