// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_WhirlwindEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"


void UAnimNotify_WhirlwindEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !WhirlwindEffect)
		return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
		return;

	UNiagaraComponent* Effect = UNiagaraFunctionLibrary::SpawnSystemAttached(
		WhirlwindEffect,
		MeshComp,
		SocketName,
		EffectLocationOffset,
		EffectRotationOffset,
		EAttachLocation::SnapToTargetIncludingScale,
		true);

	if (Effect)
	{
		Effect->SetFloatParameter(TEXT("Lifetime"), 2.f); 
	}
}
