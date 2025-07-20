// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_DrillSkill.h"
#include "Character/GPCharacterMonster.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyState_DrillSkill::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp || !EffectTemplate) return;

	SpawnedEffect = UGameplayStatics::SpawnEmitterAttached(
		EffectTemplate,
		MeshComp,
		FName("DrillSocket"),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		FVector(4.f),
		EAttachLocation::SnapToTargetIncludingScale,
		true
	);
}

void UAnimNotifyState_DrillSkill::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (SpawnedEffect && SpawnedEffect->IsActive())
	{
		SpawnedEffect->DeactivateSystem();
		SpawnedEffect = nullptr;
	}
}

