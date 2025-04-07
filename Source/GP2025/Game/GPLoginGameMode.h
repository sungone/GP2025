// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GPLoginGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPLoginGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AGPLoginGameMode();
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> LoginWidgetClass;
};
