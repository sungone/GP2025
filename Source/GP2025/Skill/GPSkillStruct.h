// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GPSkillStruct.generated.h"

USTRUCT(BlueprintType)
struct FGPSkillStruct : public FTableRowBase
{
    GENERATED_BODY()

public:
    FGPSkillStruct()
        : idx(0)
        , name(TEXT(""))
        , weapon(TEXT(""))
        , skil_group(0)
        , skill_lv(0)
        , cooltime(0.f)
        , skill_type_00(TEXT(""))
        , skill_value_00(0)
        , skill_type_01(TEXT(""))
        , skill_value_01(0)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 idx;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString weapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 skil_group;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 skill_lv;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float cooltime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString skill_type_00;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 skill_value_00;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString skill_type_01;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 skill_value_01;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 applied_Level;
};
