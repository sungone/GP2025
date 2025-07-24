// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Network/GPObjectManager.h"
#include "GameFramework/TouchInterface.h"
#include "GPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGPPlayerController();

public:
	virtual void BeginPlay() override;
};
