// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/GPLevelTransitionTrigger.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Character/GPCharacterMyplayer.h"
#include "Physics/GPCollision.h"

AGPLevelTransitionTrigger::AGPLevelTransitionTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(200.f, 200.f, 200.f));

	TriggerBox->SetCollisionProfileName(CPROFILE_GPTRIGGER);
	TriggerBox->SetGenerateOverlapEvents(true);
}

void AGPLevelTransitionTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGPLevelTransitionTrigger::OnOverlapBegin);
	}
}

void AGPLevelTransitionTrigger::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	CachedPlayer = Cast<AGPCharacterMyplayer>(OtherActor);
	if (!CachedPlayer) return;

	if (!LevelToUnload.IsNone())
	{
		UGameplayStatics::UnloadStreamLevel(this, LevelToUnload, FLatentActionInfo(), false);
	}

	if (!LevelToLoad.IsNone())
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("OnLevelLoaded");
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = __LINE__;

		UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
	}
}

void AGPLevelTransitionTrigger::OnLevelLoaded()
{
	if (!CachedPlayer || LevelToLoad.IsNone()) return;

	FVector SpawnLocation = GetSpawnLocationForLevel(LevelToLoad);
	CachedPlayer->SetActorLocation(SpawnLocation);

	UE_LOG(LogTemp, Log, TEXT("[Trigger] Player moved to spawn location after level load: %s"), *SpawnLocation.ToString());
}

FVector AGPLevelTransitionTrigger::GetSpawnLocationForLevel(FName LevelName)
{
	if (LevelName == "tip")
		return FVector(-100, 100, 150);
	if (LevelName == "E")
		return FVector(-150, 1500, 150);
	if (LevelName == "gym")
		return FVector(-2000, 0, 150);

	return FVector::ZeroVector;
}
