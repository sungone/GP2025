// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/GPSkillCoolDownHandler.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"
#include "Skill/GPSkillStruct.h"
#include "Character/GPCharacterMyplayer.h"
#include "GPSkillCoolDownHandler.h"

UGPSkillCoolDownHandler::UGPSkillCoolDownHandler()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> SkillDataTableRef(TEXT("/Game/Skill/GPSkillTable.GPSkillTable"));
    if (SkillDataTableRef.Succeeded())
    {
        SkillDataTable = SkillDataTableRef.Object;
    }
}

void UGPSkillCoolDownHandler::Init(AGPCharacterMyplayer* InOwner)
{
    UE_LOG(LogTemp, Warning, TEXT("[SkillCoolDownHandler] Init Called. Owner: %s"), *GetNameSafe(InOwner));

    Owner = InOwner;

    if (!SkillDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("[SkillCoolDownHandler] SkillDataTable is NULL during Init!"));
        return;
    }

    // SkillCooldownTimes.Empty(); // Init할 때 무조건 한번 비워줌 (중복 방지)

    TArray<FGPSkillStruct*> AllRows;
    SkillDataTable->GetAllRows(TEXT("SkillDataSearch"), AllRows);

    for (auto& Row : AllRows)
    {
        if (Row)
        {
            FSkillKey Key(Row->skil_group, Row->skill_lv);
            SkillCooldownTimes.Add(Key, Row->cooltime);

            UE_LOG(LogTemp, Warning, TEXT("[SkillCoolDownHandler] Loaded: Group=%d Level=%d Cooldown=%.2fs"), Row->skil_group, Row->skill_lv, Row->cooltime);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[SkillCoolDownHandler] Total SkillCooldownTimes: %d"), SkillCooldownTimes.Num());
}

bool UGPSkillCoolDownHandler::CanUseSkill(int32 SkillGroup, int32 SkillLevel) const
{
    if (!Owner || !IsValid(Owner))
    {
        UE_LOG(LogTemp, Error, TEXT("[SkillCoolDownHandler] Owner is NULL!"));
        return false;
    }

    if (!Owner->GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("[SkillCoolDownHandler] Owner's World is NULL!"));
        return false;
    }

    if (SkillCooldownTimes.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[SkillCoolDownHandler] SkillCooldownTimes EMPTY! Trying ReInit..."));
        const_cast<UGPSkillCoolDownHandler*>(this)->Init(Owner);
    }

    FSkillKey Key(SkillGroup, SkillLevel);

    const float* CooldownTime = SkillCooldownTimes.Find(Key);
    if (!CooldownTime)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SkillCoolDownHandler] Cooldown data not found for SkillGroup: %d, SkillLevel: %d"), SkillGroup, SkillLevel);
        return false;
    }

    const float CurrentTime = Owner->GetWorld()->GetTimeSeconds();

    const float* LastUseTime = LastSkillUseTimes.Find(Key);
    if (!LastUseTime)
    {
        return true;
    }

    float ElapsedTime = CurrentTime - *LastUseTime;

    bool bCanUse = (ElapsedTime >= *CooldownTime);
    if (!bCanUse)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SkillCoolDownHandler] Skill cooling down: %.2f/%.2f seconds elapsed (SkillGroup: %d, SkillLevel: %d)"),
            ElapsedTime, *CooldownTime, SkillGroup, SkillLevel);
    }

    return bCanUse;
}

void UGPSkillCoolDownHandler::StartCoolDown(int32 SkillGroup, int32 SkillLevel)
{
    FSkillKey Key(SkillGroup, SkillLevel);
    LastSkillUseTimes.Add(Key, Owner->GetWorld()->GetTimeSeconds());
}

float UGPSkillCoolDownHandler::GetRemainingCooldownTime(int32 SkillGroup, int32 SkillLevel) const
{
    if (!IsValid(this) || !IsValid(Owner))
    {
        UE_LOG(LogTemp, Error, TEXT("[GetRemainingCooldownTime] Invalid this or Owner"));
        return 0.f;
    }

    if (SkillCooldownTimes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GetRemainingCooldownTime] SkillCooldownTimes EMPTY!"));
        const_cast<UGPSkillCoolDownHandler*>(this)->Init(Owner);
    }

    FSkillKey Key(SkillGroup, SkillLevel);

    const float* CooldownTime = SkillCooldownTimes.Find(Key);
    const float* LastUseTime = LastSkillUseTimes.Find(Key);

    if (!CooldownTime || !LastUseTime || !Owner || !Owner->GetWorld())
    {
        return 0.0f;
    }

    float CurrentTime = Owner->GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - *LastUseTime;
    float RemainingTime = *CooldownTime - ElapsedTime;

    return FMath::Clamp(RemainingTime, 0.0f, *CooldownTime);
}

float UGPSkillCoolDownHandler::GetTotalCooldownTime(int32 SkillGroup, int32 SkillLevel) const
{
    if (!IsValid(this))
    {
        UE_LOG(LogTemp, Error, TEXT("[GetTotalCooldownTime] THIS is invalid!"));
        return 0.f;
    }

    if (!IsValid(Owner))
    {
        UE_LOG(LogTemp, Error, TEXT("[GetTotalCooldownTime] Owner is invalid!"));
        return 0.f;
    }

    if (SkillCooldownTimes.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GetTotalCooldownTime] SkillCooldownTimes is EMPTY!"));
        const_cast<UGPSkillCoolDownHandler*>(this)->Init(Owner);
    }

    FSkillKey Key(SkillGroup, SkillLevel);

    const float* CooldownTime = SkillCooldownTimes.Find(Key);

    if (!CooldownTime)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GetTotalCooldownTime] Cooldown data NOT FOUND! SkillGroup: %d, SkillLevel: %d"), SkillGroup, SkillLevel);
        return 0.f;
    }

    UE_LOG(LogTemp, Log, TEXT("[GetTotalCooldownTime] Cooldown FOUND! SkillGroup: %d, SkillLevel: %d, CooldownTime: %.2f"), SkillGroup, SkillLevel, *CooldownTime);

    return *CooldownTime;
}