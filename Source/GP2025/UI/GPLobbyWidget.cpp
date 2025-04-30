// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPLobbyWidget.h"
#include "UI/GPCharacterSelectButtonWidget.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "Network/GPNetworkManager.h"
#include "GPLobbyWidget.h"
#include "Components/Button.h"

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

void UGPLobbyWidget::OnCharacterSelected(uint8 SelectedType)
{
	using namespace Type;

	// ��Ʈ��ũ �Ŵ��� ��������
	UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (!NetMgr) return;

	if (SelectedType == static_cast<uint8>(EPlayer::WARRIOR))
	{
		// ���� ĳ���� ���� ó��
		NetMgr->SendMySelectCharacter(SelectedType);
	}
	else if (SelectedType == static_cast<uint8>(EPlayer::GUNNER))
	{
		// ���� ĳ���� ���� ó��
		NetMgr->SendMySelectCharacter(SelectedType);
	}
}

void UGPLobbyWidget::OnGameStartPressed()
{
	UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (NetMgr)
	{
		NetMgr->SendMyEnterGamePacket();
	}
}

