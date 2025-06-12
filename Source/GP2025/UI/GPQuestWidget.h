// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPQuestWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPQuestWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	// Setter Ãß°¡
	void SetQuestDescription(const FString& Description);
	void SetQuestTitle(const FString& Title);
public:

	UPROPERTY(meta = (BindWidget))
	class UButton* QuestAcceptButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuestExitButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestDescriptionText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuestTypeText;

	UPROPERTY()
	class AGPCharacterNPC* OwningNPC;

	UFUNCTION()
	void OnQuestAccepted();

	UFUNCTION()
	void OnQuestExit();

};
