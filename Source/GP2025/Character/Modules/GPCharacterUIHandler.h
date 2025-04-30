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
	void CreateCharacterStatusWidget();
	void UpdateWidgetVisibility();
	void UpdateCharacterStatus();

protected:
	class UGPWidgetComponent* CreateWidgetComponent(const FString& Name, const FString& WidgetPath, FVector Location, FVector2D Size, UUserWidget*& OutUserWidget);

public:
	UPROPERTY()
	class AGPCharacterBase* Owner;

	UPROPERTY()
	class UGPWidgetComponent* CharacterStatusWidget;

	UPROPERTY()
	class UUserWidget* CharacterStatusWidgetInstance;

	void UpdateNickNameOnly();
	//void UpdateLevelOnly();
	//void UpdateHpOnly();
};
