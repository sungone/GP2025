// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPChatMessageWidget.h"
#include "Components/TextBlock.h"

void UGPChatMessageWidget::SetChatMessage(uint8 Channel, const FString& UserName, const FString& Message)
{
	if (ChannelText)
	{
		const FString Channels[] = { TEXT("전체"), TEXT("친구"), TEXT("지역") };
		const FLinearColor Colors[] = {
			FLinearColor::White,
			FLinearColor(0.3f, 0.7f, 1.0f),
			FLinearColor(0.3f, 1.0f, 0.3f)
		};

		ChannelText->SetText(FText::FromString(TEXT("[") + Channels[Channel] + TEXT("]")));
		ChannelText->SetColorAndOpacity(FSlateColor(Colors[Channel]));
	}

	if (UserNameText)
	{
		UserNameText->SetText(FText::FromString(UserName + TEXT(":")));
	}

	if (MessageText)
	{
		MessageText->SetText(FText::FromString(Message));
	}
}
