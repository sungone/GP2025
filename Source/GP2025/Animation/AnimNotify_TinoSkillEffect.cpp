// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_TinoSkillEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_TinoSkillEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !SkillEffect) return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	UNiagaraComponent* EffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		SkillEffect,
		MeshComp,
		SocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTargetIncludingScale,
		true);

	if (EffectComp)
	{
		EffectComp->SetRelativeScale3D(FVector(2.f)); // ★ 스케일 5배 적용
	}
}

