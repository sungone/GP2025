// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPEarthQuakeRockActor.generated.h"

UCLASS()
class GP2025_API AGPEarthQuakeRockActor : public AActor
{
	GENERATED_BODY()

public:
	AGPEarthQuakeRockActor();

protected:
	virtual void BeginPlay() override;

public:
	void Init(const FVector& TargetLocation);

private:
	UPROPERTY()
	UStaticMeshComponent* Mesh;

	FVector TargetPos;
	FVector StartPos;

	UPROPERTY(EditAnywhere, Category = "Fall")
	float FallDuration = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Fall")
	int32 NumSteps = 20; 

	float ElapsedTime = 0.f;
	FTimerHandle FallTimerHandle;
	FTimerHandle DestroyTimerHandle;

	void UpdateFall();
	void DelayedDestroy();
};
