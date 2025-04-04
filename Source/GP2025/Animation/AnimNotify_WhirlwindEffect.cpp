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
		// 여기서 Duration 설정!
		Effect->SetFloatParameter(TEXT("Lifetime"), 2.f); // 원하는 재생 시간
	}
}
