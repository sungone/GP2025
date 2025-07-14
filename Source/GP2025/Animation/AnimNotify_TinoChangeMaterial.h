// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotify_TinoChangeMaterial.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UAnimNotify_TinoChangeMaterial : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// 애니메이션 시작 시 호출
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	// 애니메이션 끝날 때 호출
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	// 바꿀 머터리얼
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChangeMaterial")
	TSoftObjectPtr<UMaterialInterface> NewMaterial;

	// 원래 머터리얼 저장용
	UPROPERTY(Transient)
	UMaterialInterface* OriginalMaterial;
};
