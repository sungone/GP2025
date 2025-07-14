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
	// �ִϸ��̼� ���� �� ȣ��
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	// �ִϸ��̼� ���� �� ȣ��
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	// �ٲ� ���͸���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChangeMaterial")
	TSoftObjectPtr<UMaterialInterface> NewMaterial;

	// ���� ���͸��� �����
	UPROPERTY(Transient)
	UMaterialInterface* OriginalMaterial;
};
