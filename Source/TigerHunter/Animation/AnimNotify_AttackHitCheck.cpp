// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"
#include "Interface/THAnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		ITHAnimationAttackInterface* AttackPawn = Cast<ITHAnimationAttackInterface>(MeshComp->GetOwner());

		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
	}
}
