// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPQuestListEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPQuestListEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	//UFUNCTION(BlueprintCallable)
	//void SetQuestState(const FString& StateText);

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Quest")
	class UTextBlock* QuestStateText;
};
