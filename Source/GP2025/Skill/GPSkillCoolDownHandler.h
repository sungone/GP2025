// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPSkillCoolDownHandler.generated.h"


UCLASS()
class GP2025_API UGPSkillCoolDownHandler : public UObject
{
	GENERATED_BODY()

public:
    UGPSkillCoolDownHandler();
    void Init(class AGPCharacterMyplayer* InOwner);

    bool CanUseSkill(ESkillGroup SkillGroup, int32 SkillLevel) const;
    void StartCoolDown(ESkillGroup SkillGroup, int32 SkillLevel);


    float GetRemainingCooldownTime(ESkillGroup SkillGroup, int32 SkillLevel) const;
    float GetTotalCooldownTime(ESkillGroup SkillGroup, int32 SkillLevel) const;
    bool HasCooldownData(ESkillGroup SkillGroup, int32 SkillLevel) const;

public :
    UPROPERTY()
    AGPCharacterMyplayer* Owner;

    TMap<struct FSkillData, float> SkillCooldownTimes;
    TMap<struct FSkillData, float> LastSkillUseTimes;

    UPROPERTY(EditDefaultsOnly, Category = "SkillData")
    UDataTable* SkillDataTable;
};