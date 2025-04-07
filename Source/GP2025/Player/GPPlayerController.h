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

public:
	AGPPlayerController();

public:
	UPROPERTY()
	class UGPInGameWidget* InGameWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGPInGameWidget> InGameWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowInGameUI();
public:
	UPROPERTY()
	class UGPLoginWidget* LoginWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGPLoginWidget> LoginWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLoginUI();

public :
	UPROPERTY()
	class UGPLobbyWidget* LobbyWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGPLobbyWidget> LobbyWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowLobbyUI();
};
