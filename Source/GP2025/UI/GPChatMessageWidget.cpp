// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPChatMessageWidget.h"
#include "Components/TextBlock.h"

void UGPChatMessageWidget::SetChatMessage(EChatChannel Channel, const FString& TargetName, const FString& Message)
{
	static const TMap<EChatChannel, FString> ChannelLabels = {
		{ EChatChannel::All, TEXT("전체") },
		{ EChatChannel::Friend, TEXT("친구") },
		{ EChatChannel::Zone, TEXT("지역") },
		{ EChatChannel::Whisper, TEXT("귓속말") },
		{ EChatChannel::ItemSys, TEXT("아이템") },
		{ EChatChannel::SkillSys, TEXT("스킬") },
		{ EChatChannel::FriendSys, TEXT("친구") },
	};

	FString ChannelLabel = TEXT("알수없음");
	if (const FString* Found = ChannelLabels.Find(Channel))
	{
		ChannelLabel = *Found;
	}

	const bool bIsSystemMessage =
		(Channel == EChatChannel::ItemSys
			|| Channel == EChatChannel::SkillSys
			|| Channel == EChatChannel::FriendSys);

	const FLinearColor DefaultColor = FLinearColor::White;              // 흰색 (기본)
	const FLinearColor SystemColor = FLinearColor(1.0f, 1.0f, 0.1f);    // 연노랑

	const FLinearColor GreenColor = FLinearColor(0.0f, 1.0f, 0.0f);    // 초록
	const FLinearColor PinkColor = FLinearColor(0.9f, 0.0f, 0.3f);    // 분홍
	const FLinearColor SkyBlueColor = FLinearColor(0.0f, 0.5f, 1.0f);    // 하늘 파랑
	const FLinearColor VioletColor = FLinearColor(0.6f, 0.2f, 1.0f);    // 보라
	const FLinearColor OrangeColor = FLinearColor(1.0f, 0.3f, 0.0f);    // 주황
	const FLinearColor MintColor = FLinearColor(0.0f, 1.0f, 0.5f);    // 민트

	FLinearColor Color = DefaultColor;
	switch (Channel)
	{
	case EChatChannel::All:
		Color = OrangeColor;
		break;
	case EChatChannel::Zone:
		Color = DefaultColor;
		break;
	case EChatChannel::Friend:
		Color = SkyBlueColor;
		break;
	case EChatChannel::Whisper:
		Color = PinkColor;
		break;
	case EChatChannel::ItemSys:
	case EChatChannel::SkillSys:
	case EChatChannel::FriendSys:
		Color = SystemColor;
		break;
	default:
		Color = FLinearColor::Red;
		break;
	}

	if (ChannelText)
	{
		ChannelText->SetText(FText::FromString(FString::Printf(TEXT("[%s]"), *ChannelLabel)));
		ChannelText->SetColorAndOpacity(FSlateColor(Color));
	}

	if (TargetNameText)
	{
		TargetNameText->SetText(
			bIsSystemMessage
			? FText::FromString(TargetName)
			: FText::FromString(TargetName + TEXT(":"))
		);

		FLinearColor TargetColor = Color;

		if (ChannelText)
		{
			switch (Channel)
			{
			case EChatChannel::ItemSys:
				TargetColor = GreenColor;
				break;
			case EChatChannel::SkillSys:
				TargetColor = VioletColor;
				break;
			case EChatChannel::FriendSys:
				TargetColor = MintColor;
				break;
			default:
				break;
			}
			TargetNameText->SetColorAndOpacity(FSlateColor(TargetColor)
			);
		}

		if (MessageText)
		{
			MessageText->SetText(FText::FromString(Message));
			MessageText->SetText(FText::FromString(Message));
			MessageText->SetColorAndOpacity(FSlateColor(Color));
		}
	}

}