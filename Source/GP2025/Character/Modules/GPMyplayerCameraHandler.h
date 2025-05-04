// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPMyplayerCameraHandler.generated.h"

class AGPCharacterMyplayer;
class USpringArmComponent;
class UCameraComponent;

/**
 * 
 */
UCLASS()
class GP2025_API UGPMyplayerCameraHandler : public UObject
{
	GENERATED_BODY()
	
public:

	void Initialize(AGPCharacterMyplayer* InOwner);
	void Tick(float DeltaTime);

	// ¡‹ √≥∏Æ
	void StartZoom();
	void StopZoom();
	bool IsZooming() const;

	UFUNCTION()
	void ConfigureCameraCollision();

public:
	UPROPERTY()
	AGPCharacterMyplayer* Owner;

	bool bWantsToZoom = false;

	float DefaultFOV = 90.f;
	float ZoomedFOV = 60.f;
	float ZoomInterpSpeed = 10.f;

	FVector DefaultCameraOffset = FVector(0.f, 0.f, 150.f);
	FRotator DefaultCameraRotationOffset = FRotator(-35.f, 0.f, 0.f);
	FVector ZoomedCameraOffset = FVector(50.f, 30.f, 100.f);
	float DefaultArmLength = 600.f;
	float ZoomedArmLength = 150.f;
};
