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

	// 발사할 투사체 개수 (부채꼴 수)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	int32 NumProjectiles = 5;

	// 부채꼴의 각도 간격 (도 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float SpreadAngle = 1.2f;

	// 발사 위치 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FVector MuzzleOffset = FVector(0.f, 0.f, 0.f);
};
