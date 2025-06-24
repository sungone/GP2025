// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_FlameBreath.h"
#include "Character/GPCharacterMonster.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyState_FlameBreath::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp || !FlameEffectTemplate) return;

	SpawnedEffect = UGameplayStatics::SpawnEmitterAttached(
		FlameEffectTemplate,
		MeshComp,
		FName("MuzzleSocket"),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTargetIncludingScale,
		true 
	);
}

void UAnimNotifyState_FlameBreath::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (SpawnedEffect && SpawnedEffect->IsActive())
	{
		SpawnedEffect->DeactivateSystem();
		SpawnedEffect = nullptr;
	}
}
