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
		UE_LOG(LogTemp, Log, TEXT("[Lobby] Man Character Selected. "));
		// ToDo : ���� ĳ���� 3D �� Ȱ��ȭ �Ǵ� ���� ���� ��
		break;

	case ELobbyCharacterSelectType::WOMAN:
		UE_LOG(LogTemp, Log, TEXT("[Lobby] Woman Character Selected. "));
		// ToDo : ���� ĳ���� 3D �� Ȱ��ȭ �Ǵ� ���� ���� ��
		break;
	}
}
