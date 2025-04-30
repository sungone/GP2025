// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/GPCharacterSelectButtonWidget.h"
#include "GPLobbyWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class GP2025_API UGPLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public :
	virtual void NativeConstruct() override;

	// ���� ĳ���� ���� ��ư
	UPROPERTY(meta = (BindWidget))
	class UGPCharacterSelectButtonWidget* SelectMan;

	// ���� ĳ���� ���� ��ư
	UPROPERTY(meta = (BindWidget))
	class UGPCharacterSelectButtonWidget* SelectWoman;

	UPROPERTY(meta = (BindWidget))  
	class UButton* GameStartButton;

	// ĳ���� ���� ó��
	UFUNCTION()
	void OnCharacterSelected(uint8 NewType);

	// ���� ����
	UFUNCTION()
	void OnGameStartPressed();
private:
	enum class Type::EPlayer SelectedType = Type::EPlayer::NONE;
};
