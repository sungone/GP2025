// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_HitHardEffect.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UAnimNotify_HitHardEffect : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 부착할 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* HitHardEffect;

	// 부착할 소켓 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName SocketName = FName("WeaponSocket");

	// 오프셋 위치 (필요 시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FVector EffectLocationOffset = FVector::ZeroVector;

	// 회전 오프셋 (필요 시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FRotator EffectRotationOffset = FRotator::ZeroRotator;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
