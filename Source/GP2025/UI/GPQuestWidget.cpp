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
		// QuestTypeText->SetText(FText::FromString(TEXT("���� ����Ʈ"))); 
	}

	if (QuestDescriptionText)
	{
		// QuestDescriptionText->SetText(FText::FromString(TEXT("����ͷ� ���� ���� 10������ óġ�ϼ���.")));
	}
}

void UGPQuestWidget::OnQuestAccepted()
{
	UE_LOG(LogTemp, Log, TEXT("Quest Accepted Log!"));
}
