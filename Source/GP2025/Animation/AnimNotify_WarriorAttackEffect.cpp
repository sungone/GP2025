// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_WarriorAttackEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotify_WarriorAttackEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !AutoAttackEffect)
		return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
		return;

	UNiagaraComponent* Effect = UNiagaraFunctionLibrary::SpawnSystemAttached(
		AutoAttackEffect,
		MeshComp,
		SocketName,
		EffectLocationOffset,
		EffectRotationOffset,
		EAttachLocation::SnapToTargetIncludingScale,
		true);

	if (Effect)
	{
		// 여기서 Duration 설정!
		Effect->SetFloatParameter(TEXT("Lifetime"), 0.3f); // 원하는 재생 시간
	}
}
