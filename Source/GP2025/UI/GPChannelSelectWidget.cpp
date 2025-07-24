// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPChannelSelectWidget.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "Components/Slider.h"
#include "Character/Modules/GPMyplayerSoundManager.h"

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

void UGPChannelSelectWidget::NativeConstruct()
{
	if (ChannelComboBox)
	{
		UpdateChannelState();

		ChannelComboBox->OnSelectionChanged.AddDynamic(this, &UGPChannelSelectWidget::OnChannelSelected);
		ChannelComboBox->OnOpening.AddDynamic(this, &UGPChannelSelectWidget::UpdateChannelState);
	}

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UGPChannelSelectWidget::OnConfirmClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UGPChannelSelectWidget::OnBackClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UGPChannelSelectWidget::OnQuitClicked);
	}

	if (BGMVolume)
	{
		BGMVolume->OnValueChanged.AddDynamic(this, &UGPChannelSelectWidget::OnBGMVolumeChanged);
	}
}

void UGPChannelSelectWidget::OnChannelSelected(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	int32 SpaceIdx;
	if (SelectedItem.FindChar(TEXT(' '), SpaceIdx))
	{
		SelectedItem = SelectedItem.Left(SpaceIdx);
	}

	if (const EWorldChannel* FoundChannel = ChannelMap.Find(SelectedItem))
	{
		SelectedChannel = *FoundChannel;

		UE_LOG(LogTemp, Log, TEXT("Selected Channel: %s"), *SelectedItem);
	}
}

void UGPChannelSelectWidget::UpdateChannelState()
{
	if (ChannelComboBox)
	{
		ChannelComboBox->ClearOptions();
		ChannelComboBox->AddOption(TEXT("자동"));

		UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();

		for (int32 i = 1; i <= WORLD_MAX_COUNT; ++i)
		{
			EWorldChannel Channel = static_cast<EWorldChannel>(i);
			FString ChannelName = FString::Printf(TEXT("채널%d"), i);

			FString StatusText;
			if (NetMgr)
			{
				EWorldState State = NetMgr->GetWorldState(Channel);
				switch (State)
				{
				case EWorldState::Good:
					StatusText = TEXT(" (원활)");
					break;
				case EWorldState::Normal:
					StatusText = TEXT(" (보통)");
					break;
				case EWorldState::Bad:
					StatusText = TEXT(" (혼잡)");
					break;
				default:
					StatusText = TEXT(" ( ? )");
					break;
				}
			}
			FString FullLabel = ChannelName + StatusText;
			ChannelComboBox->AddOption(FullLabel);
		}
		EWorldChannel Ch = *NetMgr->GetMyGPChannel();
		ChannelComboBox->SetSelectedIndex(static_cast<int32>(Ch));
	}
}

void UGPChannelSelectWidget::OnConfirmClicked()
{
	if (ChannelComboBox)
	{
		UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
		if (NetMgr)
		{
			NetMgr->SendMyChangeChannelPacket(SelectedChannel);
		}
	}

	RemoveFromParent();
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
}

void UGPChannelSelectWidget::OnBackClicked()
{
	RemoveFromParent();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
}

void UGPChannelSelectWidget::OnQuitClicked()
{
	RemoveFromParent();
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->SetInputMode(FInputModeUIOnly());
		PC->SetShowMouseCursor(true);
	}
}

void UGPChannelSelectWidget::OnBGMVolumeChanged(float Value)
{
	UE_LOG(LogTemp, Log, TEXT("[ChannelSelect] BGM Volume changed: %.2f"), Value);

	if (APlayerController* PC = GetOwningPlayer())
	{
		AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(PC->GetPawn());
		if (MyPlayer && MyPlayer->SoundManager)
		{
			MyPlayer->SoundManager->SetBGMVolume(Value);
		}
	}
}
