// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPQuestListEntryWidget.h"
#include "Components/TextBlock.h"          
#include "Styling/SlateColor.h"         

void UGPQuestListEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGPQuestListEntryWidget::SetQuestState(bool bIsSuccess)
{
	if (!QuestStateText) return;

	FString StateText = bIsSuccess ? TEXT("성공") : TEXT("실패");

	QuestStateText->SetText(FText::FromString(StateText));

	FLinearColor StateColor = bIsSuccess ? FLinearColor::Green : FLinearColor::Red;
	QuestStateText->SetColorAndOpacity(FSlateColor(StateColor));
}