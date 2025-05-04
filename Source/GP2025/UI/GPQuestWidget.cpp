// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPQuestWidget.h"
#include "Components/TextBlock.h"  
#include "Components/Button.h"

void UGPQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuestAcceptButton)
	{
		QuestAcceptButton->OnClicked.AddDynamic(this, &UGPQuestWidget::OnQuestAccepted);
	}

	if (QuestTypeText)
	{
		// QuestTypeText->SetText(FText::FromString(TEXT("메인 퀘스트"))); 
	}

	if (QuestDescriptionText)
	{
		// QuestDescriptionText->SetText(FText::FromString(TEXT("사냥터로 가서 몬스터 10마리를 처치하세요.")));
	}
}

void UGPQuestWidget::OnQuestAccepted()
{
	UE_LOG(LogTemp, Log, TEXT("Quest Accepted Log!"));
}
