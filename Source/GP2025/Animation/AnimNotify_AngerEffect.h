// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AngerEffect.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UAnimNotify_AngerEffect : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* AngerEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName SocketName = FName("WeaponSocket");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float Duration = 10.f;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
