// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPPlayerEffectHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterPlayer.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

UGPPlayerEffectHandler::UGPPlayerEffectHandler()
{
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> SkillUnlockEffectFinder(TEXT("/Game/effect/ARPGEssentials/Effects/NS_ARPGEssentials_Loot_03_Rare.NS_ARPGEssentials_Loot_03_Rare"));
    if (SkillUnlockEffectFinder.Succeeded())
    {
        SkillUnlockEffect = SkillUnlockEffectFinder.Object;
    }

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> LevelUpEffectFinder(TEXT("/Game/effect/ARPGEssentials/Effects/NS_ARPGEssentials_Loot_05_Legendary.NS_ARPGEssentials_Loot_05_Legendary"));
    if (LevelUpEffectFinder.Succeeded())
    {
        LevelUpEffect = LevelUpEffectFinder.Object;
    }
}

void UGPPlayerEffectHandler::Init(AGPCharacterPlayer* InOwner)
{
	Owner = InOwner;
}

void UGPPlayerEffectHandler::PlaySkillUnlockEffect()
{
    if (Owner && SkillUnlockEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            SkillUnlockEffect,                     // ����Ʈ
            Owner->GetRootComponent(),              // �θ� ������Ʈ (��Ʈ�� ���̱�)
            TEXT("Hips"),                              // ���� �̸� (������ None)
            FVector::ZeroVector,                   
            FRotator::ZeroRotator,                
            EAttachLocation::SnapToTargetIncludingScale,  
            true                                   
        );
    }
}

void UGPPlayerEffectHandler::PlayLevelUpEffect()
{
    if (Owner && LevelUpEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            LevelUpEffect,
            Owner->GetRootComponent(),
            TEXT("Hips"),
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTargetIncludingScale,
            true
        );
    }
}
