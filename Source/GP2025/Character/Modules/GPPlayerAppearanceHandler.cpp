// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Character/GPCharacterMyPlayer.h"
#include "Character/GPCharacterControlData.h"
#include "Item/GPItemStruct.h"
#include "Weapons/GPWeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneCaptureComponent2D.h"

void UGPPlayerAppearanceHandler::Initialize(AGPCharacterPlayer* InOwner)
{
	Owner = InOwner;

	if (Owner && Owner->GetMesh())
	{
		Owner->GetMesh()->SetSkeletalMesh(nullptr);
	}
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
		Owner->Helmet->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
		Owner->Helmet->SetVisibility(true);
	}
	else if (Owner->Helmet)
	{
		Owner->Helmet->SetSkeletalMesh(nullptr);
		Owner->Helmet->SetVisibility(false);
	}

	EquipWeaponFromData(CharacterData);
	SetupLeaderPose();
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

USkeletalMesh* UGPPlayerAppearanceHandler::GetBodyMeshByCharacterType(const FGPItemStruct& ItemData, uint8 CurrentCharacterType)
{
	using namespace Type;

	if (CurrentCharacterType == (uint8)EPlayer::GUNNER)
	{
		return ItemData.ItemSkeletalMesh_Woman;
	}
	else if (CurrentCharacterType == (uint8)EPlayer::WARRIOR)
	{
		return ItemData.ItemSkeletalMesh_Man;
	}

	return nullptr;
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
			Owner->Helmet->SetupAttachment(Owner->BodyMesh);
			Owner->Helmet->RegisterComponent();
		}

		if (ItemData.ItemSkeletalMesh)
		{
			Owner->Helmet->SetSkeletalMesh(ItemData.ItemSkeletalMesh);
			Owner->Helmet->SetVisibility(true);
		}
		else
		{
			Owner->Helmet->SetSkeletalMesh(nullptr);
			Owner->Helmet->SetVisibility(false);
		}

		if (Owner->BodyMesh && Owner->BodyMesh->DoesSocketExist(TEXT("HelmetSocket")))
		{
			if (Owner->Helmet->GetAttachParent() != Owner->BodyMesh)
			{
				Owner->Helmet->AttachToComponent(
					Owner->BodyMesh,
					FAttachmentTransformRules::SnapToTargetIncludingScale,
					TEXT("HelmetSocket"));
			}

			FTimerHandle TimerHandle;
			Owner->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
				{
					Owner->Helmet->SetLeaderPoseComponent(Owner->BodyMesh, true);
				}, 0.05f, false);
		}
	}

	if (ItemData.Category == ECategory::chest)
	{

		USkeletalMesh* MeshToApply = GetBodyMeshByCharacterType(ItemData, Owner->CurrentCharacterType);
		if (!MeshToApply)
			return;

		TSubclassOf<UAnimInstance> PreviousAnimBP = nullptr;
		if (Owner->BodyMesh)
		{
			PreviousAnimBP = Owner->BodyMesh->GetAnimClass();
			Owner->BodyMesh->DestroyComponent();
			Owner->BodyMesh = nullptr;
		}

		Owner->BodyMesh = NewObject<USkeletalMeshComponent>(Owner);
		Owner->BodyMesh->SetSkeletalMesh(MeshToApply);
		Owner->BodyMesh->SetupAttachment(Owner->GetMesh());
		Owner->BodyMesh->RegisterComponent();

		if (PreviousAnimBP)
		{
			Owner->BodyMesh->SetAnimInstanceClass(PreviousAnimBP);
		}

		SetupLeaderPose();
		AttachWeaponToBodyMesh();
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

void UGPPlayerAppearanceHandler::SetupLeaderPose()
{
	if (!Owner || !Owner->BodyMesh) return;

	FTimerHandle TimerHandle;
	Owner->GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			if (Owner->HeadMesh)
			{
				Owner->HeadMesh->SetLeaderPoseComponent(Owner->BodyMesh, true);
			}
			if (Owner->LegMesh)
			{
				Owner->LegMesh->SetLeaderPoseComponent(Owner->BodyMesh, true);
			}
			if (Owner->Helmet)
			{
				Owner->Helmet->SetLeaderPoseComponent(Owner->BodyMesh, true);
				if (Owner->BodyMesh->DoesSocketExist(TEXT("HelmetSocket")))
				{
					Owner->Helmet->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
					Owner->Helmet->SetVisibility(true);
				}
			}

		}, 0.05f, false);
}

void UGPPlayerAppearanceHandler::AttachWeaponToBodyMesh()
{
	if (Owner->WeaponActor && Owner->WeaponActor->GetWeaponMesh() && Owner->BodyMesh)
	{
		Owner->WeaponActor->GetWeaponMesh()->AttachToComponent(
			Owner->BodyMesh,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("WeaponSocket"));
	}
}
