// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../GP_Server/Source/Common/Common.h"
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

	void SetQuestState(bool bIsSuccess);
	void SetQuestTask(uint8 InQuestType);

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Quest")
	class UTextBlock* QuestStateText;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, Category = "Quest")
	class UTextBlock* QuestTaskText;

	// QuestType EntryType;
	uint8 EntryType;
};
