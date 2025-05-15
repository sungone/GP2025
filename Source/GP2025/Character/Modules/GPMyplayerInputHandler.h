// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InputActionValue.h"
#include "GPMyplayerInputHandler.generated.h"

class UEnhancedInputComponent;
class AGPCharacterMyplayer;
class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class GP2025_API UGPMyplayerInputHandler : public UObject
{
	GENERATED_BODY()

public :
	UGPMyplayerInputHandler();
public:
    void Initialize(AGPCharacterMyplayer* InOwner, UEnhancedInputComponent* InInputComponent);
	void SetupInputBindings(UEnhancedInputComponent* EnhancedInput);

public :
	UPROPERTY()
	TObjectPtr<AGPCharacterMyplayer> Owner;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY()
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY()
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY()
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY()
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY()
	TObjectPtr<UInputAction> AutoAttackAction;

	UPROPERTY()
	TObjectPtr<UInputAction> InventoryAction;

	UPROPERTY()
	TObjectPtr<UInputAction> SettingAction;

	UPROPERTY()
	TObjectPtr<UInputAction> TakeInteractionAction;

	UPROPERTY()
	TObjectPtr<UInputAction> ZoomAction;

	UPROPERTY()
	TObjectPtr<UInputAction> SkillQAction;

	UPROPERTY()
	TObjectPtr<UInputAction> SkillEAction;

	UPROPERTY()
	TObjectPtr<UInputAction> SkillRAction;

	UPROPERTY()
	TObjectPtr<UInputAction> AcceptAction;

	UPROPERTY()
	TObjectPtr<UInputAction> RefuseAction;

	UPROPERTY()
	TObjectPtr<UInputAction> InteractionAction;

	UPROPERTY()
	TObjectPtr<UInputAction> EnterKeyAction;

public :
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

    void Jump();
    void StopJumping();

    void StartSprinting();
    void StopSprinting();

    void AutoAttack();

    void ToggleInventory();
    void ResetInventoryToggle();
	void OpenInventory();
	void CloseInventory();

    void OpenSettingWidget();

	void Accept();
	void Refuse();
	void Interact();
	void EnterChatting();
	

	void TakeInteraction();
	UPROPERTY()
	AActor* CurrentInteractionTarget = nullptr;

    void StartAiming();
    void StopAiming();

	// <Skill>
    void UseSkillQ();
    void UseSkillE();
    void UseSkillR();

public :
	bool bGetTakeItem = false;
	FTimerHandle GetInteractionResetTimerHandle;

private:
	bool bCanJump = true;
	FTimerHandle JumpCooldownTimerHandle;

};
