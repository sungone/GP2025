// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPSkillCoolDownHandler.generated.h"


/**
 * 
 */


USTRUCT(BlueprintType)
struct FSkillKey
{
    GENERATED_BODY()

    UPROPERTY()
    int32 SkillGroup;

    UPROPERTY()
    int32 SkillLevel;

    FSkillKey() : SkillGroup(0), SkillLevel(0) {}
    FSkillKey(int32 InGroup, int32 InLevel) : SkillGroup(InGroup), SkillLevel(InLevel) {}

    bool operator==(const FSkillKey& Other) const
    {
        return SkillGroup == Other.SkillGroup && SkillLevel == Other.SkillLevel;
    }
};

FORCEINLINE uint32 GetTypeHash(const FSkillKey& Key)
{
    return HashCombine(::GetTypeHash(Key.SkillGroup), ::GetTypeHash(Key.SkillLevel));
}

UCLASS()
class GP2025_API UGPSkillCoolDownHandler : public UObject
{
	GENERATED_BODY()

public:
    UGPSkillCoolDownHandler();
    void Init(class AGPCharacterMyplayer* InOwner);

    bool CanUseSkill(int32 SkillGroup, int32 SkillLevel) const;
    void StartCoolDown(int32 SkillGroup, int32 SkillLevel);

    float GetRemainingCooldownTime(int32 SkillGroup, int32 SkillLevel) const;
    float GetTotalCooldownTime(int32 SkillGroup, int32 SkillLevel) const;

public:
    UPROPERTY()
    AGPCharacterMyplayer* Owner;

    UPROPERTY()
    TMap<struct FSkillKey, float> SkillCooldownTimes;

    UPROPERTY()
    TMap<struct FSkillKey, float> LastSkillUseTimes;

    UPROPERTY(EditDefaultsOnly, Category = "SkillData")
    UDataTable* SkillDataTable;
};