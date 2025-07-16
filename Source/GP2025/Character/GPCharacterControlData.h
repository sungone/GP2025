// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GPCharacterControlData.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPCharacterControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UGPCharacterControlData();

	UPROPERTY(EditAnywhere, Category = "Pawn")
	uint32 bUseControllerRotationYaw : 1;

	UPROPERTY(EditAnywhere, Category = "CharacterMovement")
	uint32 bOrientRotationToMovement : 1;

	UPROPERTY(EditAnywhere, Category = "CharacterMovement")
	uint32 bUseControllerDesiredRotation : 1;

	UPROPERTY(EditAnywhere, Category = "CharacterMovement")
	FRotator RotationRate;

	// Character Movement Section
	UPROPERTY(EditAnywhere, Category = "Character Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, Category = "Character Movement")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> MobileMappingContext;

	UPROPERTY(EditAnywhere, Category = "SpringArm")
	FRotator RelativeRotation;

	UPROPERTY(EditAnywhere, Category = "SpringArm")
	uint32 bUsePawnControlRotation : 1;

	UPROPERTY(EditAnywhere, Category = "SpringArm")
	uint32 bInheritPitch : 1;

	UPROPERTY(EditAnywhere, Category = "SpringArm")
	uint32 bInheritYaw : 1;

	UPROPERTY(EditAnywhere, Category = "SpringArm")
	uint32 bInheritRoll : 1;

	UPROPERTY(EditAnywhere, Category = "SpringArm")
	uint32 bDoCollisionTest : 1;

	/// 스켈레탈 매쉬 , 애니메이션 블루프린트 & 몽타주 설정
	UPROPERTY(EditAnywhere, Category = "Mesh")
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, Category = "Animation")
	TSubclassOf<UAnimInstance> AnimBlueprint;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackWithoutWeaponAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackAnimMontage_2;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* AttackAnimMontage_3;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* QSkillAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* ESkillAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* RSkillAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* DeadAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* MonsterHitAnimMontage;

	// 장비 아이템 설정
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	USkeletalMesh* HelmetMesh;

	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	USkeletalMesh* BodyMesh;

	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	USkeletalMesh* LegMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UStaticMesh* WeaponMesh;

	// 몬스터의 캡슐 세팅
	UPROPERTY(EditAnywhere, Category = "Capsule")
	float CapsuleHalfHeight = 99.f;
	  
	UPROPERTY(EditAnywhere, Category = "Capsule")
	float CapsuleRadius = 42.f;

	UPROPERTY(EditAnywhere, Category = "Capsule")
	uint8 bIsBoos : 1;
};
