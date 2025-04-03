// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ThrowingEffect.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UAnimNotify_ThrowingEffect : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<AActor> ProjectileEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FVector MuzzleOffset = FVector(0.f, -30.f, 80.f);

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
