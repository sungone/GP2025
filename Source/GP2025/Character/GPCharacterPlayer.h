// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "InputActionValue.h"
#include "GPCharacterPlayer.generated.h"

/**
 * 
 */

UENUM()
enum class ECharacterPlayerControlType : uint8
{
	Default,
	Aim
};

UCLASS()
class GP2025_API AGPCharacterPlayer : public AGPCharacterBase
{
	GENERATED_BODY()

public :
	AGPCharacterPlayer();

protected :
	virtual void BeginPlay() override;

public :
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// Character Control Section
	void SetCharacterControl(ECharacterPlayerControlType NewCharacterPlayerControlType);
	virtual void SetCharacterControlData(const class UGPCharacterPlayerControlData* CharacterPlayerControlData);

	UPROPERTY(EditAnywhere , Category = "CharacterControl" , Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterPlayerControlType, class UGPCharacterPlayerControlData*> CharacterPlayerControlManager;

// Camera Section
protected :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera" , Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

// Input Section
protected :
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input" , Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

// Character Movement Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 1500.f;


// Control Function
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	virtual void StopJumping() override;

private :
	FVector PreviousLocation;
	bool bWasJumping = false;

	// Sprint Functions
	void StartSprinting();
	void StopSprinting();

};
