// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "InputActionValue.h"
#define PACKETSENDTIME 0.5
#include "GPCharacterPlayer.h"
#include "GPCharacterMyplayer.generated.h"


/**
 *
 */

UCLASS()
class GP2025_API AGPCharacterMyplayer : public AGPCharacterPlayer, public IGPAnimationAttackInterface
{
	GENERATED_BODY()

public:
	AGPCharacterMyplayer();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void SetCharacterType(ECharacterType NewCharacterControlType) override;
	virtual void SetCharacterData(const class UGPCharacterControlData* CharacterControlData) override;

protected:
	// Input �Լ�
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	virtual void StopJumping() override;
	void StartSprinting();
	void StopSprinting();
	void AutoAttack();
	void ToggleInventory();
	void OpenInventory();
	void CloseInventory();
	void ResetInventoryToggle();
	void OpenSettingWidget();
	bool bInventoryToggled = false;

	// ī�޶� 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// Input ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AutoAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> SettingAction;

	// Sprint Speed ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float SprintSpeed;

public :
	// Inventory Widget
	UPROPERTY()
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UFUNCTION()
	UGPInventory* GetInventoryWidget();

	// Setting Widget
	UPROPERTY()
	TSubclassOf<UUserWidget> SettingWidgetClass;

	UPROPERTY()
	UUserWidget* SettingWidget;

	// InGame Widget
	UPROPERTY()
	TSubclassOf<UUserWidget> InGameWidgetClass;

	UPROPERTY()
	UUserWidget* InGameWidget;


	// Attack Hit Section
protected:
	virtual void AttackHitCheck() override;

	// �ٸ� ��� Ŭ���̾�Ʈ���� ��ġ ����ȭ�� ���� ��ġ�� ����ϴ� ������
public:
	FVector LastLocation;
	float LastRotationYaw;
	FInfoData LastSendPlayerInfo;

public:
	float MovePacketSendTimer = PACKETSENDTIME; // �������� ��Ŷ ��ȯ �ð�
	float GroundZLocation = 147.7;
	bool isJumpStart = false;
	bool bWasJumping = false;

	// CharacterInfo
	virtual void SetCharacterInfo(FInfoData& CharacterInfo_) override;
};
