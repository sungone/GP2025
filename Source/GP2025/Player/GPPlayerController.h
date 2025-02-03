// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected :
	AGPPlayerController();
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly , Category = "UI")
	TSubclassOf<UUserWidget> PlayerMainWidgetClass;

	UPROPERTY()
	UUserWidget* PlayerMainWidget;
};
