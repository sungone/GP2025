// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPMyplayerUIManager.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPMyplayerUIManager : public UObject
{
	GENERATED_BODY()

public :
	UGPMyplayerUIManager();
public:
	void Initialize(class AGPCharacterMyplayer* InOwner);

	// Login
	void OnSetUpInGameWidgets();

	// Inventory
	UFUNCTION()
	void ToggleInventory();
	
	UFUNCTION()
	void ResetToggleInventory();

	UFUNCTION()
	void OpenInventory();

	UFUNCTION()
	void CloseInventory();

	// Setting
	void OpenSettingWidget();

	// InGame
	void ShowInGameUI();

public:
	UPROPERTY()
	AGPCharacterMyplayer* Owner;

	UPROPERTY()
	UUserWidget* InventoryWidget;

	UPROPERTY()
	UUserWidget* SettingWidget;

	UPROPERTY()
	UUserWidget* InGameWidget;

	UPROPERTY()
	UUserWidget* GunCrosshairWidget;

	UPROPERTY()
	UUserWidget* LobbyWidget;

	UPROPERTY()
	UUserWidget* LoginWidget;

	UPROPERTY()
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> SettingWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> InGameWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> LoginWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> GunCrosshairWidgetClass;

	bool bInventoryToggled = false;

public :
	class UGPInGameWidget* GetInGameWidget();
	class UGPInventory* GetInventoryWidget();

public :
	void ShowLobbyUI();
	void ShowLoginUI();
};

