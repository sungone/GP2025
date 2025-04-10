// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPCharacterUIHandler.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPCharacterUIHandler : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(class AGPCharacterBase* InOwner);
	void OnBeginPlay();

	void CreateAllWidgets();
	void UpdateAllWidgetVisibility();

	void SetupNickNameUI();

public:
	UPROPERTY()
	AGPCharacterBase* Owner;

	UPROPERTY()
	class UGPWidgetComponent* HpBar;

	UPROPERTY()
	class UGPWidgetComponent* LevelText;

	UPROPERTY()
	class UGPWidgetComponent* NickNameText;

	UUserWidget* HpBarWidget;
	UUserWidget* LevelTextWidget;
	UUserWidget* NickNameWidget;

	UGPWidgetComponent* CreateWidgetComponent(const FString& Name, const FString& WidgetPath, FVector Location, FVector2D Size, UUserWidget*& OutUserWidget);
};
