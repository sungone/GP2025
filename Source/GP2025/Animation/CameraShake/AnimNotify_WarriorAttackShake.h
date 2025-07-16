// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_WarriorAttackShake.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UAnimNotify_WarriorAttackShake : public UAnimNotify
{
	GENERATED_BODY()
	
public:
    // �����Ϳ��� ������ �� �ֵ��� CameraShake Ŭ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
    TSubclassOf<class UCameraShakeBase> CameraShakeClass;

    // ��鸲 ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
    float Scale = 1.0f;

    // ��鸲 ���ӽð�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
    float Duration = 0.2f;

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
