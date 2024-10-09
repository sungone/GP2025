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

	// 플레이어의 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FVector Velocity;

	// 땅에서의 속력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float GroundSpeed;

	// Idle 판별
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsIdle : 1;

	// 움직이고 있는지 쉬고 있는지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float MovingThreshould;

	// 떨어지는 지 판별
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsFalling : 1;

	// 점프하는 지 판별
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	uint8 bIsJumping : 1;

	// 점프 중인지 판별
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float JumpingThreshould;
};
