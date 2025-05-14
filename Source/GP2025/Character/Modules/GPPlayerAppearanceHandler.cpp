// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPPlayerAppearanceHandler.h"
#include "Character/GPCharacterMyPlayer.h"
#include "Character/GPCharacterControlData.h"
#include "Item/GPItemStruct.h"
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

	if (Owner && Owner->WeaponMesh)
	{
		Owner->WeaponMesh->SetStaticMesh(nullptr);
		Owner->WeaponMesh->SetVisibility(false);
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

	if (CharacterData->WeaponMesh && Owner->WeaponMesh)
	{
		Owner->WeaponMesh->SetStaticMesh(CharacterData->WeaponMesh);
		Owner->WeaponMesh->SetVisibility(true);
	}
	else
	{
		Owner->WeaponMesh->SetStaticMesh(nullptr);
		Owner->WeaponMesh->SetVisibility(false);
	}

	SetupLeaderPose();
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

		if (Owner->HelmetMesh)
			Owner->HelmetMesh->SetRelativeLocation(FVector(0.f, 0.f, -4.f));
	}

	// 무기 교체
	if (ItemData.Category == ECategory::sword || ItemData.Category == ECategory::bow)
	{
		Owner->WeaponMesh->SetStaticMesh(ItemData.ItemStaticMesh);
		Owner->WeaponMesh->SetVisibility(ItemData.ItemStaticMesh != nullptr);
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

		break;
	}

	case ECategory::sword:
	case ECategory::bow:
		if (Owner->WeaponMesh)
		{
			Owner->WeaponMesh->SetStaticMesh(nullptr);
			Owner->WeaponMesh->SetVisibility(false);
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