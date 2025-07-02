// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_FThrowingEffect.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UAnimNotify_FThrowingEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<AActor> ProjectileEffectClass;

	// �߻��� ����ü ���� (��ä�� ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	int32 NumProjectiles = 5;

	// ��ä���� ���� ���� (�� ����)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float SpreadAngle = 1.2f;

	// �߻� ��ġ ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FVector MuzzleOffset = FVector(0.f, 0.f, 0.f);
};
