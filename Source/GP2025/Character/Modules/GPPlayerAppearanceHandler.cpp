// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Character/GPCharacterPlayer.h"
#include "Character/GPCharacterControlData.h"
#include "Item/GPItemStruct.h"
#include "Weapons/GPWeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

void UGPPlayerAppearanceHandler::Initialize(AGPCharacterPlayer* InOwner)
{
	Owner = InOwner;
}

void UGPPlayerAppearanceHandler::ApplyCharacterPartsFromData(const UGPCharacterControlData* CharacterData)
{
	if (!Owner || !CharacterData) return;

	if (CharacterData->BodyMesh && Owner->BodyMesh)
	{
		TSubclassOf<UAnimInstance> PrevAnimBP = Owner->BodyMesh->GetAnimClass();
		Owner->BodyMesh->SetSkeletalMesh(CharacterData->BodyMesh);
		if (PrevAnimBP)
		{
			Owner->BodyMesh->SetAnimInstanceClass(PrevAnimBP);
		}
	}

	if (CharacterData->HeadMesh && Owner->HeadMesh)
	{
		Owner->HeadMesh->SetSkeletalMesh(CharacterData->HeadMesh);
	}

	if (CharacterData->LegMesh && Owner->LegMesh)
	{
		Owner->LegMesh->SetSkeletalMesh(CharacterData->LegMesh);
	}

	if (CharacterData->HelmetMesh && Owner->Helmet)
	{
		Owner->Helmet->SetSkeletalMesh(CharacterData->HelmetMesh);
		Owner->Helmet->AttachToComponent(Owner->HeadMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
		Owner->Helmet->SetVisibility(true);
	}
	else if (Owner->Helmet)
	{
		Owner->Helmet->SetSkeletalMesh(nullptr);
		Owner->Helmet->SetVisibility(false);
	}

	EquipWeaponFromData(CharacterData);
	SetupMasterPose();
}

void UGPPlayerAppearanceHandler::EquipWeaponFromData(const UGPCharacterControlData* CharacterData)
{
	if (!Owner || !CharacterData) return;

	if (Owner->WeaponActor)
	{
		Owner->WeaponActor->Destroy();
		Owner->WeaponActor = nullptr;
	}

	if (CharacterData->WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Owner;

		Owner->WeaponActor = Owner->GetWorld()->SpawnActor<AGPWeaponBase>(CharacterData->WeaponClass, Owner->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
		if (Owner->WeaponActor && Owner->BodyMesh && Owner->BodyMesh->DoesSocketExist(TEXT("WeaponSocket")))
		{
			Owner->WeaponActor->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
			Owner->WeaponActor->SetWeaponMesh(CharacterData->WeaponMesh);
		}
	}
}

void UGPPlayerAppearanceHandler::EquipItemOnCharacter(FGPItemStruct& ItemData)
{
	if (!Owner) return;

	UE_LOG(LogTemp, Warning, TEXT("Equipped : %s"), *ItemData.ItemName.ToString());

	if (ItemData.Category == ECategory::helmet)
	{
		if (!Owner->Helmet)
		{
			Owner->Helmet = NewObject<USkeletalMeshComponent>(Owner);
			Owner->Helmet->SetupAttachment(Owner->HeadMesh);
			Owner->Helmet->RegisterComponent();
		}

		Owner->Helmet->SetSkeletalMesh(ItemData.ItemSkeletalMesh);

		if (Owner->BodyMesh)
		{
			Owner->Helmet->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
			Owner->Helmet->SetMasterPoseComponent(Owner->BodyMesh);
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
		if (Owner->BodyMesh)
		{
			PreviousAnimBP = Owner->BodyMesh->GetAnimClass();
			Owner->BodyMesh->DestroyComponent();
			Owner->BodyMesh = nullptr;
		}

		Owner->BodyMesh = NewObject<USkeletalMeshComponent>(Owner);
		Owner->BodyMesh->SetSkeletalMesh(ItemData.ItemSkeletalMesh);
		Owner->BodyMesh->SetupAttachment(Owner->GetMesh());
		Owner->BodyMesh->RegisterComponent();

		if (PreviousAnimBP)
		{
			Owner->BodyMesh->SetAnimInstanceClass(PreviousAnimBP);
		}

		SetupMasterPose();

		if (Owner->HeadMesh)
		{
			Owner->HeadMesh->SetMasterPoseComponent(Owner->BodyMesh);
		}
		if (Owner->LegMesh)
		{
			Owner->LegMesh->SetMasterPoseComponent(Owner->BodyMesh);
		}
		if (Owner->Helmet)
		{
			Owner->Helmet->SetMasterPoseComponent(Owner->BodyMesh);

			if (Owner->BodyMesh->DoesSocketExist(TEXT("HelmetSocket")))
			{
				Owner->Helmet->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
				Owner->Helmet->SetVisibility(true);
				UE_LOG(LogTemp, Warning, TEXT("Helmet Re-Attached to New BodyMesh"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("HelmetSocket does not exist on new BodyMesh!"));
			}
		}

		if (Owner->WeaponActor)
		{
			Owner->WeaponActor->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
			UE_LOG(LogTemp, Warning, TEXT("Weapon successfully equipped on BodyMesh: %s"), *ItemData.ItemName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn WeaponActor! Check if WeaponClass is valid."));
		}
	}

	if (ItemData.Category == ECategory::sword || ItemData.Category == ECategory::bow)
	{
		if (Owner->WeaponActor)
		{
			Owner->WeaponActor->Destroy();
			Owner->WeaponActor = nullptr;
		}

		if (!Owner->BodyMesh)
		{
			UE_LOG(LogTemp, Error, TEXT("BodyMesh is NULL! Weapon cannot be equipped."));
			return;
		}

		if (!Owner->BodyMesh->DoesSocketExist(TEXT("WeaponSocket")))
		{
			UE_LOG(LogTemp, Error, TEXT("WeaponSocket does not exist on BodyMesh!"));
			return;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Owner;
		Owner->WeaponActor = Owner->GetWorld()->SpawnActor<AGPWeaponBase>(ItemData.WeaponClass, Owner->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

		if (Owner->WeaponActor)
		{
			Owner->WeaponActor->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
			UE_LOG(LogTemp, Warning, TEXT("Weapon successfully equipped on BodyMesh: %s"), *ItemData.ItemName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn WeaponActor! Check if WeaponClass is valid."));
		}
	}
}

void UGPPlayerAppearanceHandler::SetupMasterPose()
{
	if (!Owner || !Owner->BodyMesh) return;

	if (Owner->HeadMesh)
	{
		Owner->HeadMesh->SetMasterPoseComponent(Owner->BodyMesh);
	}
	if (Owner->LegMesh)
	{
		Owner->LegMesh->SetMasterPoseComponent(Owner->BodyMesh);
	}
	if (Owner->Helmet)
	{
		Owner->Helmet->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
		Owner->Helmet->SetMasterPoseComponent(Owner->BodyMesh);
	}
	if (Owner->WeaponActor && Owner->WeaponActor->GetWeaponMesh())
	{
		UStaticMeshComponent* WeaponMesh = Owner->WeaponActor->GetWeaponMesh();
		WeaponMesh->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
	}
}