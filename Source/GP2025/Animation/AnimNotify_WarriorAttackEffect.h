// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_WarriorAttackEffect.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UAnimNotify_WarriorAttackEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* AutoAttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName SocketName = FName("WeaponSocket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FVector EffectLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FRotator EffectRotationOffset = FRotator::ZeroRotator;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
