// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPLobbyWidget.h"
#include "UI/GPCharacterSelectButtonWidget.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPLobbyWidget.h"

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
}

void UGPLobbyWidget::OnCharacterSelected(uint8 SelectedType)
{
	using namespace Type;

	if (SelectedType == static_cast<uint8>(EPlayer::WARRIOR))
	{
		// 남자 캐릭터 선택 처리
	}
	else if (SelectedType == static_cast<uint8>(EPlayer::GUNNER))
	{
		// 여자 캐릭터 선택 처리
	}
	else
	{
		// 기존 캐릭터 처리
	}
}

