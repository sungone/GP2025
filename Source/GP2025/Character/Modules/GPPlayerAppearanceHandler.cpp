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

	if (CharacterData->HelmetMesh && Owner->HelmetMesh)
	{
		Owner->HelmetMesh->SetSkeletalMesh(CharacterData->HelmetMesh);
	}

	if (CharacterData->LegMesh && Owner->LegMesh)
	{
		Owner->LegMesh->SetSkeletalMesh(CharacterData->LegMesh);
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

	// Helmet 장착 처리
	if (ItemData.Category == ECategory::helmet && Owner->HelmetMesh)
	{
		USkeletalMesh* MeshToApply = GetBodyMeshByCharacterType(ItemData, Owner->CurrentCharacterType);
		if (!MeshToApply || !Owner->HelmetMesh) return;

		Owner->HelmetMesh->SetSkeletalMesh(MeshToApply);
		Owner->HelmetMesh->SetVisibility(MeshToApply != nullptr);

		SetupLeaderPose();
		AttachWeaponToBodyMesh();
	}

	// Chest 교체 시 메시만 바꿔끼움
	if (ItemData.Category == ECategory::chest)
	{
		USkeletalMesh* MeshToApply = GetBodyMeshByCharacterType(ItemData, Owner->CurrentCharacterType);
		if (!MeshToApply || !Owner->BodyMesh) return;

		Owner->BodyMesh->SetSkeletalMesh(MeshToApply);

		if (Owner->BodyMesh->GetAnimInstance() == nullptr && Owner->GetMesh()->GetAnimClass())
		{
			Owner->BodyMesh->SetAnimInstanceClass(Owner->GetMesh()->GetAnimClass());
		}

		SetupLeaderPose();
		AttachWeaponToBodyMesh();

		if (Owner->HelmetMesh)
			Owner->HelmetMesh->SetRelativeLocation(FVector(0.f, 0.f, -4.f));
	}

	// 무기 교체
	if (ItemData.Category == ECategory::sword || ItemData.Category == ECategory::bow)
	{
		if (Owner->WeaponActor)
		{
			Owner->WeaponActor->Destroy();
			Owner->WeaponActor = nullptr;
		}

		if (!ItemData.WeaponClass || !Owner->BodyMesh) return;

		if (!Owner->BodyMesh->DoesSocketExist(TEXT("WeaponSocket")))
		{
			UE_LOG(LogTemp, Error, TEXT("WeaponSocket not found on BodyMesh."));
			return;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Owner;

		Owner->WeaponActor = Owner->GetWorld()->SpawnActor<AGPWeaponBase>(
			ItemData.WeaponClass,
			Owner->GetActorLocation(),
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (Owner->WeaponActor)
		{
			Owner->WeaponActor->AttachToComponent(
				Owner->BodyMesh,
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				TEXT("WeaponSocket")
			);
			Owner->WeaponActor->SetWeaponMesh(ItemData.ItemStaticMesh);
		}
	}
}

void UGPPlayerAppearanceHandler::UnequipItemFromCharacter(ECategory Category)
{
	if (!Owner) return;

	switch (Category)
	{
	case ECategory::helmet:
	{
		auto* ControlData = Owner->CharacterTypeManager.Find(Owner->CurrentCharacterType);
		if (ControlData && *ControlData && (*ControlData)->IsValidLowLevelFast())
		{
			Owner->HelmetMesh->SetSkeletalMesh((*ControlData)->HelmetMesh);
		}

		SetupLeaderPose();
		AttachWeaponToBodyMesh();
		break;
	}

	case ECategory::chest:
	{
		// 안전성 체크 예시
		auto* ControlData = Owner->CharacterTypeManager.Find(Owner->CurrentCharacterType);
		if (ControlData && *ControlData && (*ControlData)->IsValidLowLevelFast())
		{
			Owner->BodyMesh->SetSkeletalMesh((*ControlData)->BodyMesh);
		}
		if (Owner->GetMesh()->GetAnimClass())
		{
			Owner->BodyMesh->SetAnimInstanceClass(Owner->GetMesh()->GetAnimClass());
		}
		SetupLeaderPose();
		AttachWeaponToBodyMesh();

		break;
	}

	case ECategory::sword:
	case ECategory::bow:
		if (Owner->WeaponActor)
		{
			Owner->WeaponActor->Destroy();
			Owner->WeaponActor = nullptr;
		}
		break;

	default:
		break;
	}
}

void UGPPlayerAppearanceHandler::SetupLeaderPose()
{
	if (!Owner || !Owner->BodyMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("SetupLeaderPose Update Failed"));
		return;
	}
	 
	if (Owner->HelmetMesh)
	{
		Owner->HelmetMesh->SetLeaderPoseComponent(Owner->BodyMesh, true);
	}
	if (Owner->LegMesh)
	{
		Owner->LegMesh->SetLeaderPoseComponent(Owner->BodyMesh, true);
	}
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
