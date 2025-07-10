// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPool/GPItemPool.h"
#include "Engine/World.h"

void UGPItemPool::Initialize(UWorld* InWorld, int32 InitialSize)
{
	World = InWorld;
	ItemClass = AGPItem::StaticClass();

	for (int i = 0; i < InitialSize; i++)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AGPItem* NewItem = World->SpawnActor<AGPItem>(
			ItemClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			Params
		);

		NewItem->SetActorHiddenInGame(true);
		NewItem->SetActorEnableCollision(false);
		NewItem->SetActorTickEnabled(false);
		NewItem->SetPool(this);
		NewItem->Reset();

		Pool.Add(NewItem);
	}
}

AGPItem* UGPItemPool::Acquire()
{
	if (Pool.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemPool] Pool exhausted. Spawning new AGPItem."));

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AGPItem* NewItem = World->SpawnActor<AGPItem>(
			ItemClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			Params
		);

		NewItem->SetPool(this);
		return NewItem;
	}

	AGPItem* Item = Pool.Pop();
	Item->SetActorHiddenInGame(false);
	Item->SetActorEnableCollision(true);
	Item->SetActorTickEnabled(true);

	return Item;
}

void UGPItemPool::Release(AGPItem* Item)
{
	if (!Item)
		return;

	Item->Reset();
	Pool.Add(Item);
}