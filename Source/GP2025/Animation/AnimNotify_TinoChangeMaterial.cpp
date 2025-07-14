// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_TinoChangeMaterial.h"

void UAnimNotify_TinoChangeMaterial::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp) return;

	// 기존 머터리얼 저장
	OriginalMaterial = MeshComp->GetMaterial(0);

	// 새로운 머터리얼 로드
	if (NewMaterial.IsValid())
	{
		MeshComp->SetMaterial(0, NewMaterial.Get());
	}
	else
	{
		NewMaterial.LoadSynchronous();
		if (NewMaterial.IsValid())
		{
			MeshComp->SetMaterial(0, NewMaterial.Get());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("TinoChangeMaterial NotifyBegin - Changed material."));
}

void UAnimNotify_TinoChangeMaterial::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	// 원래 머터리얼로 복원
	if (OriginalMaterial)
	{
		MeshComp->SetMaterial(0, OriginalMaterial);
		UE_LOG(LogTemp, Log, TEXT("TinoChangeMaterial NotifyEnd - Restored original material."));
	}
}
