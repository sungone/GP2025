// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_HitHardEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_HitHardEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !HitHardEffect)
		return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
		return;

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		HitHardEffect,
		MeshComp,
		SocketName,
		EffectLocationOffset,
		EffectRotationOffset,
		EAttachLocation::SnapToTargetIncludingScale,
		true);
}
