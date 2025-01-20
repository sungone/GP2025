// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GPCharacterBase.h"
#include "InputActionValue.h"
#define PACKETSENDTIME 0.5
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

public:
	AGPCharacterPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetCharacterControl(ECharacterType NewCharacterControlType) override;
	virtual void SetCharacterControlData(const class UGPCharacterControlData* CharacterControlData) override;

	// Input �Լ�
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	virtual void StopJumping() override;
	void StartSprinting();
	void StopSprinting();
	void AutoAttack();
	void ChangeCharacterControl();

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
	TObjectPtr<class UInputAction> ChangeCharacterTypeAction;

	// Sprint Speed ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement", Meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 1500.f;

	// �ٸ� ��� Ŭ���̾�Ʈ���� ��ġ ����ȭ�� ���� ��ġ�� ����ϴ� ������
public:
	FVector LastLocation;
	float LastRotationYaw;
	FCharacterInfo LastSendPlayerInfo;

public:
	float MovePacketSendTimer = PACKETSENDTIME; // �������� ��Ŷ ��ȯ �ð�
	float GroundZLocation = 147.7;
	bool isJumpStart = false;


// ������ �浹ó��
public :
	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
