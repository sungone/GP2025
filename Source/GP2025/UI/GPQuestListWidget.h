// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPQuestListWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPQuestListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct();

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UGPQuestListEntryWidget* TinoQuest;

public:
	UFUNCTION(BlueprintCallable)
	void ShowQuestEntry(const FString& QuestID);
};
