// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "THPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TIGERHUNTER_API ATHPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected :
	virtual void BeginPlay() override;
};
