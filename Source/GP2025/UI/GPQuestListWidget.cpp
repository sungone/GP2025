// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPQuestListWidget.h"
#include "UI/GPQuestListEntryWidget.h"

void UGPQuestListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TinoQuest)
	{
		TinoQuest->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGPQuestListWidget::ShowQuestEntry(const FString& QuestID)
{
	if (QuestID == "TinoQuest" && TinoQuest)
	{
		TinoQuest->SetVisibility(ESlateVisibility::Visible);
	}
}
