// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GPPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public :
	UGPPlayerAnimInstance();

protected :
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<class UCharacterMovementComponent> Movement;

	// �÷��̾��� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FVector Velocity;

	// �������� �ӷ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float GroundSpeed;

	// Idle �Ǻ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsIdle : 1;

	// �����̰� �ִ��� ���� �ִ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float MovingThreshould;

	// �������� �� �Ǻ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsFalling : 1;

	// �����ϴ� �� �Ǻ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsJumping : 1;

	// ���� ������ �Ǻ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float JumpingThreshould;
};
