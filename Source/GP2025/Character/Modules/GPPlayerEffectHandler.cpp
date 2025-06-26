// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPPlayerEffectHandler.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterPlayer.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Boss/GPEarthQuakeRockActor.h"

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
            SkillUnlockEffect,                     // 이펙트
            Owner->GetRootComponent(),              // 부모 컴포넌트 (루트에 붙이기)
            TEXT("Hips"),                              // 소켓 이름 (없으면 None)
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

void UGPPlayerEffectHandler::PlayEarthQuakeRock(const FVector& RockPos)
{
    if (!Owner)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayEarthQuakeRock] Owner is nullptr"));
        return;
    }

    if (!EarthQuakeRockActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayEarthQuakeRock] EarthQuakeRockActorClass is nullptr"));
        return;
    }

    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayEarthQuakeRock] World is nullptr"));
        return;
    }

    FVector StartLocation = RockPos + FVector(0.f, 0.f, 800.f);
    FRotator Rotation = FRotator::ZeroRotator;

    UE_LOG(LogTemp, Log, TEXT("[PlayEarthQuakeRock] Try to spawn rock. Start: %s, Target: %s"),
        *StartLocation.ToString(),
        *RockPos.ToString());

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* SpawnedRock = World->SpawnActor<AActor>(
        EarthQuakeRockActorClass,
        StartLocation,
        Rotation,
        SpawnParams
    );

    if (!SpawnedRock)
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayEarthQuakeRock] SpawnActor FAILED!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[PlayEarthQuakeRock] Rock actor spawned successfully: %s"), *SpawnedRock->GetName());

    if (AGPEarthQuakeRockActor* Rock = Cast<AGPEarthQuakeRockActor>(SpawnedRock))
    {
        Rock->Init(RockPos);
        UE_LOG(LogTemp, Log, TEXT("[PlayEarthQuakeRock] Rock Init() called with TargetPos: %s"), *RockPos.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[PlayEarthQuakeRock] Cast to AGPEarthQuakeRockActor FAILED!"));
    }
    
}