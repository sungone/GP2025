// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPChatMessageWidget.h"
#include "Components/TextBlock.h"

void UGPChatMessageWidget::SetChatMessage(uint8 Channel, const FString& UserName, const FString& Message)
{
	if (ChannelText)
	{
		const FString Channels[] = { TEXT("전체"), TEXT("친구"), TEXT("지역"), TEXT("아이템"), TEXT("스킬"), TEXT("친구") };
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
			ChannelColor = FLinearColor(1.0f, 0.9f, 0.4f); 
			break;
		case 4: // 스킬
			ChannelColor = FLinearColor(0.f, 0.f, 0.9f); 
			break;
		default:
			ChannelColor = FLinearColor::Red;
			break;
		}

		ChannelText->SetText(FText::FromString(TEXT("[") + ChannelLabel + TEXT("]")));
		ChannelText->SetColorAndOpacity(FSlateColor(ChannelColor));
	}

	if (UserNameText)
	{
		if (Channel == 5 || Channel == 3 || Channel == 4) // 친구 , 아이템 , 스킬
		{
			UserNameText->SetText(FText::FromString(UserName));
		}
		else
		{
			UserNameText->SetText(FText::FromString(UserName + TEXT(":")));
		}
	}

	if (MessageText)
	{
		MessageText->SetText(FText::FromString(Message));
	}
}
