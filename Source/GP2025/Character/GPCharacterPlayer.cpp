// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Character/GPCharacterControlData.h"
#include "Network/GPGameInstance.h"
#include "Item/GPItemStruct.h"
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

        if (Helmet)
        {
            Helmet->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
            Helmet->SetMasterPoseComponent(BodyMesh);
            UE_LOG(LogTemp, Warning, TEXT("MasterPose Applied to Helmet"));
        }

        if (WeaponActor && WeaponActor->GetWeaponMesh())
        {
            UStaticMeshComponent* WeaponMesh = WeaponActor->GetWeaponMesh();
            WeaponMesh->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
            UE_LOG(LogTemp, Warning, TEXT("Weapon Attached to BodyMesh -> WeaponSocket"));
        }
    }

    UE_LOG(LogTemp, Log, TEXT("SetupMasterPose() Completed"));
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

void AGPCharacterPlayer::EquipItemOnCharacter(FGPItemStruct& ItemData)
{
    UE_LOG(LogTemp, Warning, TEXT("Equipped : %s"), *ItemData.ItemName.ToString());

    if (ItemData.Category == ECategory::helmet)
    {
        if (!Helmet)
        {
            Helmet = NewObject<USkeletalMeshComponent>(this);
            Helmet->SetupAttachment(HeadMesh);
            Helmet->RegisterComponent();
        }

        Helmet->SetSkeletalMesh(ItemData.ItemSkeletalMesh);

        if (BodyMesh)
        {
            Helmet->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
            Helmet->SetMasterPoseComponent(BodyMesh);
            UE_LOG(LogTemp, Warning, TEXT("Helmet Re-Attached to BodyMesh"));
        }
    }


    if (ItemData.Category == ECategory::chest)
    {
        if (!ItemData.ItemSkeletalMesh)
        {
            UE_LOG(LogTemp, Error, TEXT("ItemSkeletalMesh is NULL for chest armor!"));
            return;
        }

        TSubclassOf<UAnimInstance> PreviousAnimBP = nullptr;
        if (BodyMesh)
        {
            PreviousAnimBP = BodyMesh->GetAnimClass();
            BodyMesh->DestroyComponent();
            BodyMesh = nullptr;
        }

        BodyMesh = NewObject<USkeletalMeshComponent>(this);
        BodyMesh->SetSkeletalMesh(ItemData.ItemSkeletalMesh);
        BodyMesh->SetupAttachment(GetCapsuleComponent());
        BodyMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
        BodyMesh->RegisterComponent();

        if (PreviousAnimBP)
        {
            BodyMesh->SetAnimInstanceClass(PreviousAnimBP);
        }

        SetupMasterPose();


        if (HeadMesh)
        {
            HeadMesh->SetMasterPoseComponent(BodyMesh);
        }

        if (LegMesh)
        {
            LegMesh->SetMasterPoseComponent(BodyMesh);
        }

        if (Helmet)
        {
            Helmet->SetMasterPoseComponent(BodyMesh);

            if (BodyMesh->DoesSocketExist(TEXT("HelmetSocket")))
            {
                Helmet->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
                Helmet->SetVisibility(true);
                UE_LOG(LogTemp, Warning, TEXT("Helmet Re-Attached to New BodyMesh"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("HelmetSocket does not exist on new BodyMesh!"));
            }
        }

        if (WeaponActor)
        {
            WeaponActor->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
            UE_LOG(LogTemp, Warning, TEXT("Weapon successfully equipped on BodyMesh: %s"), *ItemData.ItemName.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn WeaponActor! Check if WeaponClass is valid."));
        }
    }


    if (ItemData.Category == ECategory::sword || ItemData.Category == ECategory::bow)
    {
        if (WeaponActor)
        {
            WeaponActor->Destroy();
            WeaponActor = nullptr;
        }

        if (!BodyMesh)
        {
            UE_LOG(LogTemp, Error, TEXT("BodyMesh is NULL! Weapon cannot be equipped."));
            return;
        }

        if (!BodyMesh->DoesSocketExist(TEXT("WeaponSocket")))
        {
            UE_LOG(LogTemp, Error, TEXT("WeaponSocket does not exist on BodyMesh!"));
            return;
        }

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        WeaponActor = GetWorld()->SpawnActor<AGPWeaponBase>(ItemData.WeaponClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

        if (WeaponActor)
        {
            WeaponActor->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
            UE_LOG(LogTemp, Warning, TEXT("Weapon successfully equipped on BodyMesh: %s"), *ItemData.ItemName.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to spawn WeaponActor! Check if WeaponClass is valid."));
        }
    }
}