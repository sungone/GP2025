// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPDeadScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPDeadScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable)
	void OnRespawnButtonClicked();
};
