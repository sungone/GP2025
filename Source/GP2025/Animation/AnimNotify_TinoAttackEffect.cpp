// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_TinoAttackEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_TinoAttackEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !AttackEffect) return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	UNiagaraComponent* EffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		AttackEffect,
		MeshComp,
		SocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTargetIncludingScale,
		true);

	if (EffectComp)
	{
		EffectComp->SetRelativeScale3D(FVector(1.3f)); 
	}
}

