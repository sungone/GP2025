// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AngerEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_AngerEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !AngerEffect)
		return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
		return;

	UNiagaraComponent* EffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		AngerEffect,
		MeshComp,
		SocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTargetIncludingScale,
		true);

	if (EffectComp)
	{
		EffectComp->SetFloatParameter(TEXT("Duration"), Duration);
	}
}
