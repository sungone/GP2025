// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_TinoAttackEffect.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UAnimNotify_TinoAttackEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* AttackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName SocketName = FName("HipsSocket");

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
