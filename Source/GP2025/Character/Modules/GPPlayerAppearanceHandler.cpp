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
		// 캐릭터 타입 로그 출력
		UE_LOG(LogTemp, Warning, TEXT("CurrentCharacterType: %d"), Owner->CurrentCharacterType);

		// 메시 선택
		USkeletalMesh* MeshToApply = GetBodyMeshByCharacterType(ItemData, Owner->CurrentCharacterType);

		// 메시가 어떤 건지 로그 출력
		if (MeshToApply)
		{
			UE_LOG(LogTemp, Warning, TEXT("MeshToApply: %s"), *MeshToApply->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No valid MeshToApply found for CharacterType %d"), Owner->CurrentCharacterType);
			return;
		}

		// 이전 AnimBP 백업
		TSubclassOf<UAnimInstance> PreviousAnimBP = nullptr;
		if (Owner->BodyMesh)
		{
			PreviousAnimBP = Owner->BodyMesh->GetAnimClass();
			Owner->BodyMesh->DestroyComponent();
			Owner->BodyMesh = nullptr;
		}

		// 새로운 BodyMesh 생성
		Owner->BodyMesh = NewObject<USkeletalMeshComponent>(Owner);
		Owner->BodyMesh->SetSkeletalMesh(MeshToApply);
		Owner->BodyMesh->SetupAttachment(Owner->GetMesh());
		Owner->BodyMesh->RegisterComponent();

		if (PreviousAnimBP)
		{
			Owner->BodyMesh->SetAnimInstanceClass(PreviousAnimBP);
		}

		if (Owner->SceneCaptureComponent)
		{
			Owner->SceneCaptureComponent->ShowOnlyComponents.Add(Owner->BodyMesh);
		}

		SetupMasterPose();
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
		Owner->Helmet->SetMasterPoseComponent(Owner->BodyMesh);

		if (Owner->BodyMesh->DoesSocketExist(TEXT("HelmetSocket")))
		{
			Owner->Helmet->AttachToComponent(Owner->BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
			Owner->Helmet->SetVisibility(true);
		}
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
