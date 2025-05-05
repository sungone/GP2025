// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPQuestListEntryWidget.h"
#include "Components/TextBlock.h"          
#include "Styling/SlateColor.h"         

void UGPQuestListEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGPQuestListEntryWidget::SetQuestState(const FString& StateText)
{
	if (!QuestStateText) return;

	QuestStateText->SetText(FText::FromString(StateText));

	FLinearColor StateColor = FLinearColor::Yellow; 

	if (StateText == TEXT("Success"))
	{
		StateColor = FLinearColor::Green;
	}
	else if (StateText == TEXT("Failed"))
	{
		StateColor = FLinearColor::Red;
	}

	QuestStateText->SetColorAndOpacity(FSlateColor(StateColor));
}
