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

	// 남자 캐릭터 선택 버튼
	UPROPERTY(meta = (BindWidget))
	class UGPCharacterSelectButtonWidget* SelectMan;

	// 여자 캐릭터 선택 버튼
	UPROPERTY(meta = (BindWidget))
	class UGPCharacterSelectButtonWidget* SelectWoman;

	UPROPERTY(meta = (BindWidget))  
	class UButton* GameStartButton;

	// 캐릭터 선택 처리
	UFUNCTION()
	void OnCharacterSelected(uint8 NewType);

	// 게임 시작
	UFUNCTION()
	void OnGameStartPressed();
private:
	enum class Type::EPlayer SelectedType = Type::EPlayer::NONE;
};
