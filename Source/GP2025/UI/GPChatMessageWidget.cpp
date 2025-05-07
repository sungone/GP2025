// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPChatMessageWidget.h"
#include "Components/TextBlock.h"

void UGPChatMessageWidget::SetChatMessage(const FString& UserName, const FString& Message)
{
	if (UserNameText)
	{
		UserNameText->SetText(FText::FromString(UserName + TEXT(":")));
	}

	if (MessageText)
	{
		MessageText->SetText(FText::FromString(Message));
	}
}