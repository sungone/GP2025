// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/GPFloatingDamageTextPool.h"
#include "Engine/World.h"

void UGPFloatingDamageTextPool::Initialize(UWorld* InWorld, int32 InitialSize)
{
	World = InWorld;
	DamageTextClass = AGPFloatingDamageText::StaticClass();

    for (int i = 0; i < InitialSize; i++)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AGPFloatingDamageText* NewDamageText = World->SpawnActor<AGPFloatingDamageText>(
            DamageTextClass,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            Params
        );

        NewDamageText->SetActorHiddenInGame(true);
        NewDamageText->SetActorEnableCollision(false);
        NewDamageText->SetActorTickEnabled(false);
        NewDamageText->SetPool(this);
        NewDamageText->Reset();

        Pool.Add(NewDamageText);
    }
}

AGPFloatingDamageText* UGPFloatingDamageTextPool::Acquire()
{
    if (Pool.Num() == 0)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AGPFloatingDamageText* NewDamageText = World->SpawnActor<AGPFloatingDamageText>(
            DamageTextClass,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            Params
        );

        NewDamageText->SetPool(this);
        return NewDamageText;
    }

    AGPFloatingDamageText* Text = Pool.Pop();
    Text->SetActorHiddenInGame(false);
    Text->SetActorEnableCollision(true);
    Text->SetActorTickEnabled(true);

    return Text;
}

void UGPFloatingDamageTextPool::Release(AGPFloatingDamageText* DamageText)
{
    if (!DamageText)
        return;

    DamageText->Reset(); // 내부 상태 초기화 필요하다면 직접 구현

    Pool.Add(DamageText);
}