// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPPlayerEffectHandler.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPPlayerEffectHandler : public UObject
{
	GENERATED_BODY()

public:
    UGPPlayerEffectHandler();
    void Init(class AGPCharacterPlayer* InOwner);

    UFUNCTION()
    void PlaySkillUnlockEffect();

    UFUNCTION()
    void PlayLevelUpEffect();

    UFUNCTION()
    void PlayQuestClearEffect();

    UFUNCTION()
    void PlayPlayerHitEffect();

    UFUNCTION()
    void PlayPlayerCriticalEffect();

    UFUNCTION()
    void PlayHealEffect();

    UFUNCTION()
    void PlayAttackBuffEffect();
public:
    UPROPERTY()
    AGPCharacterPlayer* Owner;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class UNiagaraSystem* SkillUnlockEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class UNiagaraSystem* LevelUpEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    class UNiagaraSystem* QuestClearEffect;

    void PlayEarthQuakeRock(const FVector& RockPos);

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TSubclassOf<class AGPEarthQuakeRockActor> EarthQuakeRockActorClass;

    UPROPERTY(EditAnywhere, Category = "Effect")
    class UNiagaraSystem* HitEffect;

    UPROPERTY(EditAnywhere, Category = "Effect")
    class UNiagaraSystem* CriticalEffect;

    UPROPERTY(EditAnywhere, Category = "Effect")
    UNiagaraSystem* HealEffect;

    UPROPERTY(EditAnywhere, Category = "Effect")
    UNiagaraSystem* AttackBuffEffect;
};
