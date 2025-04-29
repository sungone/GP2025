// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPLobbyWidget.h"
#include "UI/GPCharacterSelectButtonWidget.h"

void UGPLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectMan)
	{
		SelectMan->CharacterType = ELobbyCharacterSelectType::MAN;
		SelectMan->OnCharacterSelected.AddDynamic(this, &UGPLobbyWidget::OnCharacterSelected);
	}

	if (SelectWoman)
	{
		SelectWoman->CharacterType = ELobbyCharacterSelectType::WOMAN;
		SelectWoman->OnCharacterSelected.AddDynamic(this, &UGPLobbyWidget::OnCharacterSelected);
	}
}

void UGPLobbyWidget::OnCharacterSelected(ELobbyCharacterSelectType SelectedType)
{
	switch (SelectedType)
	{
	case ELobbyCharacterSelectType::MAN:
		// ToDo : 남자 캐릭터 3D 모델 활성화 또는 서버 전송 등
		break;

	case ELobbyCharacterSelectType::WOMAN:
		// ToDo : 여자 캐릭터 3D 모델 활성화 또는 서버 전송 등
		break;
	}
}
