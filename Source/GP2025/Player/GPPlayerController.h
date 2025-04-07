// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Network/GPObjectManager.h"
#include "GPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API AGPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected :
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
public:
	AGPPlayerController();

public:
	UPROPERTY()
	class UGPInGameWidget* InGameWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGPInGameWidget> InGameWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInGameUI();

public :
	UPROPERTY()
	class UGPLobbyWidget* LobbyWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGPLobbyWidget> LobbyWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLobbyUI();
};
