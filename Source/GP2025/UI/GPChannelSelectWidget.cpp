// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPChannelSelectWidget.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "Components/Slider.h"
#include "Character/Modules/GPMyplayerSoundManager.h"

void UGPChannelSelectWidget::NativeConstruct()
{
	ChannelComboBox->OnSelectionChanged.AddDynamic(this, &UGPChannelSelectWidget::OnChannelSelected);
	ChannelComboBox->AddOption(TEXT("자동"));
	ChannelComboBox->AddOption(TEXT("1채널"));
	ChannelComboBox->AddOption(TEXT("2채널"));
	ChannelComboBox->AddOption(TEXT("3채널"));
	ChannelComboBox->AddOption(TEXT("4채널"));
	ChannelComboBox->AddOption(TEXT("5채널"));
	ChannelComboBox->AddOption(TEXT("6채널"));
	ChannelComboBox->AddOption(TEXT("7채널"));
	ChannelComboBox->AddOption(TEXT("8채널"));
	ChannelComboBox->SetSelectedIndex(0);

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

}

void UGPChannelSelectWidget::OnConfirmClicked()
{
	if (ChannelComboBox)
	{
		int32 SelectedIndex = ChannelComboBox->FindOptionIndex(ChannelComboBox->GetSelectedOption());
		EWorldChannel SelectedChannel = ConvertIndexToChannel(SelectedIndex);
		UE_LOG(LogTemp, Log, TEXT("[ChannelSelectWidget] Confirmed Index: %d, Enum: %d"), SelectedIndex, static_cast<uint8>(SelectedChannel));

		UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
		if (NetMgr)
		{
			NetMgr->SendMyChangeChannelPacket(SelectedChannel);
		}
	}

	RemoveFromParent();
}

EWorldChannel UGPChannelSelectWidget::ConvertIndexToChannel(int32 Index)
{
	if (Index <= 0)
	{
		return EWorldChannel::None; 
	}
	else if (Index >= 1 && Index <= 8)
	{
		return static_cast<EWorldChannel>(static_cast<uint8>(EWorldChannel::TUWorld_1) + Index - 1);
	}
	return EWorldChannel::None;
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
