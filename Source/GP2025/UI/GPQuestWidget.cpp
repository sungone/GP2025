// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPQuestWidget.h"
#include "Components/TextBlock.h"  
#include "Components/Button.h"
#include "Character/GPCharacterNPC.h"

void UGPQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuestAcceptButton)
	{
		QuestAcceptButton->OnClicked.AddDynamic(this, &UGPQuestWidget::OnQuestAccepted);
		QuestExitButton->OnClicked.AddDynamic(this, &UGPQuestWidget::OnQuestExit);
	}
}

void UGPQuestWidget::OnQuestAccepted()
{
	UE_LOG(LogTemp, Log, TEXT("Quest Accepted Log!"));
}

void UGPQuestWidget::OnQuestExit()
{
	if (OwningNPC)
	{
		OwningNPC->ExitInteraction();
		RemoveFromParent();
	}
}
