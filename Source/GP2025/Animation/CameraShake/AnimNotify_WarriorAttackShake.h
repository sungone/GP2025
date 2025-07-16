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
    // 에디터에서 선택할 수 있도록 CameraShake 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
    TSubclassOf<class UCameraShakeBase> CameraShakeClass;

    // 흔들림 세기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
    float Scale = 1.0f;

    // 흔들림 지속시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraShake")
    float Duration = 0.2f;

    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
