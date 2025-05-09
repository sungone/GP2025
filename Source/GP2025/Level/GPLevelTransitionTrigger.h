// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPLevelTransitionTrigger.generated.h"

UCLASS()
class GP2025_API AGPLevelTransitionTrigger : public AActor
{
	GENERATED_BODY()
	
public:
	AGPLevelTransitionTrigger();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere)
	FName LevelToLoad;

	UPROPERTY(EditAnywhere)
	FName LevelToUnload;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

public:
	UPROPERTY()
	class AGPCharacterMyplayer* CachedPlayer;

	UPROPERTY(VisibleAnywhere, Category = "Effect")
	class UNiagaraComponent* PortalEffect;


public :
	UFUNCTION()
	void OnLevelRemoved(ULevel* Level, UWorld* World);

	UFUNCTION()
	void OnLevelAdded(ULevel* Level, UWorld* World);
};
