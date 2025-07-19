// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPLobbyWidget.h"
#include "UI/GPCharacterSelectButtonWidget.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "Network/GPNetworkManager.h"
#include "GPLobbyWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ComboBoxString.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"

void UGPLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectMan)
	{
		SelectMan->CharacterType = static_cast<uint8>(Type::EPlayer::WARRIOR);
		SelectMan->OnCharacterSelected.AddDynamic(this, &UGPLobbyWidget::OnCharacterSelected);
	}

	if (SelectWoman)
	{
		SelectWoman->CharacterType = static_cast<uint8>(Type::EPlayer::GUNNER);
		SelectWoman->OnCharacterSelected.AddDynamic(this, &UGPLobbyWidget::OnCharacterSelected);
	}

	if (GameStartButton)
	{
		GameStartButton->OnClicked.AddDynamic(this, &UGPLobbyWidget::OnGameStartPressed);
	}

	if (ChannelComboBox)
	{
		ChannelComboBox->ClearOptions();
		ChannelComboBox->AddOption(TEXT("채널1"));
		ChannelComboBox->AddOption(TEXT("채널2"));
		ChannelComboBox->AddOption(TEXT("채널3"));
		ChannelComboBox->AddOption(TEXT("채널4"));
		ChannelComboBox->AddOption(TEXT("채널5"));
		ChannelComboBox->AddOption(TEXT("채널6"));
		ChannelComboBox->AddOption(TEXT("채널7"));
		ChannelComboBox->AddOption(TEXT("채널8"));

		ChannelComboBox->SetSelectedIndex(0);
		ChannelComboBox->OnSelectionChanged.AddDynamic(this, &UGPLobbyWidget::OnChannelChanged);
	}
}

void UGPLobbyWidget::OnCharacterSelected(uint8 NewType)
{
	SelectedType = static_cast<Type::EPlayer>(NewType);

	FString SelectedImagePath;

	if (SelectedType == Type::EPlayer::WARRIOR)

	{
		SelectedImagePath = ManImagePath;
	}
	else if (SelectedType == Type::EPlayer::GUNNER)
	{
		SelectedImagePath = WomanImagePath;
	}

	UpdatePreviewImage(SelectedImagePath);
}

void UGPLobbyWidget::UpdatePreviewImage(const FString& ImagePath)
{
	if (!PreviewImage) return;

	// 이미지 로드
	UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, *ImagePath);
	if (NewTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(NewTexture);
		PreviewImage->SetBrush(Brush);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load texture at path: %s"), *ImagePath);
	}
}

void UGPLobbyWidget::OnGameStartPressed()
{
	UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (NetMgr)
	{
		if (SelectedType != Type::EPlayer::NONE)
			NetMgr->SendMyEnterGamePacket(SelectedChannel, SelectedType);
		else
			NetMgr->SendMyEnterGamePacket(SelectedChannel);
	}

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
}

void UGPLobbyWidget::OnChannelChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	static const TMap<FString, EWorldChannel> ChannelMap = {
		{ TEXT("채널1"), EWorldChannel::TUWorld_1 },
		{ TEXT("채널2"), EWorldChannel::TUWorld_2 },
		{ TEXT("채널3"), EWorldChannel::TUWorld_3 },
		{ TEXT("채널4"), EWorldChannel::TUWorld_4 },
		{ TEXT("채널5"), EWorldChannel::TUWorld_5 },
		{ TEXT("채널6"), EWorldChannel::TUWorld_6 },
		{ TEXT("채널7"), EWorldChannel::TUWorld_7 },
		{ TEXT("채널8"), EWorldChannel::TUWorld_8 },
	};

	if (const EWorldChannel* FoundChannel = ChannelMap.Find(SelectedItem))
	{
		SelectedChannel = *FoundChannel;
		UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
		if (NetMgr)
		{
			EWorldState State = NetMgr->GetWorldState(SelectedChannel);
			FLinearColor Color = GetColorByWorldState(State);
			SetChannelTextColor(Color);
		}

		UE_LOG(LogTemp, Log, TEXT("Selected Channel: %s"), *SelectedItem);
	}
}

FLinearColor UGPLobbyWidget::GetColorByWorldState(EWorldState State)
{
	switch (State)
	{
	case EWorldState::Good:
		return FLinearColor::Green;
	case EWorldState::Normal:
		return FLinearColor(1.f, 0.5f, 0.f); // 주황
	case EWorldState::Bad:
		return FLinearColor::Red;
	default:
		return FLinearColor::White;
	}
}


void UGPLobbyWidget::SetChannelTextColor(FLinearColor NewColor)
{
	if (!ChannelComboBox) return;

	FComboBoxStyle StyleCopy = ChannelComboBox->WidgetStyle;

	FButtonStyle ButtonStyle = StyleCopy.ComboButtonStyle.ButtonStyle;

	ButtonStyle.Normal.TintColor = FSlateColor(NewColor);
	ButtonStyle.Hovered.TintColor = FSlateColor(NewColor);
	ButtonStyle.Pressed.TintColor = FSlateColor(NewColor);

	StyleCopy.ComboButtonStyle.ButtonStyle = ButtonStyle;
	ChannelComboBox->WidgetStyle = StyleCopy;

	UE_LOG(LogTemp, Log, TEXT("R=%.2f G=%.2f B=%.2f"), NewColor.R, NewColor.G, NewColor.B);
}
