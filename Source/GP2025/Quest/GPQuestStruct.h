// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GPQuestStruct.generated.h"


USTRUCT(BlueprintType)
struct FGPQuestStruct : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Chapter;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite)
    //QuestType QuestName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite)
    //EQuestCategory Category;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TargetID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ExpReward;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GoldReward;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RewardItemTypeID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NextQuestID;
};