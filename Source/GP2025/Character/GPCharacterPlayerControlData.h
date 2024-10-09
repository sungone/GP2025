// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GPCharacterPlayerControlData.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPCharacterPlayerControlData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public :
	UGPCharacterPlayerControlData();

	UPROPERTY(EditAnywhere, Category = "Pawn")
	uint32 bUseControllerRotationYaw : 1;
	
	UPROPERTY(EditAnywhere, Category = "CharacterMovement")
	uint32 bOrientRotationToMovement : 1;

	UPROPERTY(EditAnywhere, Category = "CharacterMovement")
	uint32 bUseControllerDesiredRotation : 1;

	UPROPERTY(EditAnywhere, Category = "CharacterMovement")
	FRotator RotationRate;

	// Character Movement Section
	UPROPERTY(EditAnywhere , Category = "Character Movement")
	float WalkSpeed;

	UPROPERTY(EditAnywhere , Category = "Character Movement")
	float SprintSpeed;

	UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category = "SpringArm")
	float TargetArmLength;

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
};
