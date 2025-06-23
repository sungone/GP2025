// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPLobbyWidget.h"
#include "UI/GPCharacterSelectButtonWidget.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "Network/GPNetworkManager.h"
#include "GPLobbyWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
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
			NetMgr->SendMyEnterGamePacket(SelectedType);
		else
			NetMgr->SendMyEnterGamePacket();
	}

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
}