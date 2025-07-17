// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPChatMessageWidget.h"
#include "Components/TextBlock.h"

void UGPChatMessageWidget::SetChatMessage(uint8 Channel, const FString& UserName, const FString& Message)
{
	if (ChannelText)
	{
		const FString Channels[] = { TEXT("전체"), TEXT("친구"), TEXT("지역"), TEXT("아이템") };
		const int32 NumChannels = UE_ARRAY_COUNT(Channels);

		FString ChannelLabel = (Channel < NumChannels) ? Channels[Channel] : TEXT("알 수 없음");
		FLinearColor ChannelColor = FLinearColor::White;

		switch (Channel)
		{
		case 0: // 전체
			ChannelColor = FLinearColor::White;
			break;
		case 1: // 친구
			ChannelColor = FLinearColor(0.3f, 0.7f, 1.0f);
			break;
		case 2: // 지역
			ChannelColor = FLinearColor(0.3f, 1.0f, 0.3f);
			break;
		case 3: // 아이템
			ChannelColor = FLinearColor(1.0f, 0.9f, 0.4f); // 노란색 톤 제안
			break;
		default:
			ChannelColor = FLinearColor::Red;
			break;
		}

		ChannelText->SetText(FText::FromString(TEXT("[") + Channels[Channel] + TEXT("]")));
		ChannelText->SetColorAndOpacity(FSlateColor(ChannelColor));
	}

	if (UserNameText)
	{
		if (Channel != 3) 
		{
			UserNameText->SetText(FText::FromString(UserName + TEXT(":")));
		}
		else // 아이템
		{
			UserNameText->SetText(FText::FromString(UserName));
		}
	}

	if (MessageText)
	{
		MessageText->SetText(FText::FromString(Message));
	}
}
