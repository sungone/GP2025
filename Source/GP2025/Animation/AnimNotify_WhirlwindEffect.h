// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_WhirlwindEffect.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UAnimNotify_WhirlwindEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	// ������ ����Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* WhirlwindEffect;

	// ������ ���� �̸�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName SocketName = FName("WeaponSocket");

	// ������ ��ġ (�ʿ� ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FVector EffectLocationOffset = FVector::ZeroVector;

	// ȸ�� ������ (�ʿ� ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FRotator EffectRotationOffset = FRotator::ZeroRotator;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
