// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/GPCharacterSelectButtonWidget.h"
#include "GPLobbyWidget.generated.h"

class UButton;
class UImage;

/**
 * 
 */
UCLASS()
class GP2025_API UGPLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 남자 캐릭터 선택 버튼
	UPROPERTY(meta = (BindWidget))
	class UGPCharacterSelectButtonWidget* SelectMan;

	// 여자 캐릭터 선택 버튼
	UPROPERTY(meta = (BindWidget))
	class UGPCharacterSelectButtonWidget* SelectWoman;

	UPROPERTY(meta = (BindWidget))
	class UButton* GameStartButton;

	UPROPERTY(meta = (BindWidget))
	UImage* PreviewImage;

	// 남자 캐릭터 이미지 경로
	UPROPERTY(EditAnywhere, Category = "Preview")
	FString ManImagePath;

	// 여자 캐릭터 이미지 경로
	UPROPERTY(EditAnywhere, Category = "Preview")
	FString WomanImagePath;

	// 캐릭터 선택 처리
	UFUNCTION()
	void OnCharacterSelected(uint8 NewType);

	/** 이미지 변경 함수 */
	void UpdatePreviewImage(const FString& ImagePath);

	// 게임 시작
	UFUNCTION()
	void OnGameStartPressed();
	UFUNCTION()
	void UpdateChannelState();
private:
	Type::EPlayer SelectedType = Type::EPlayer::NONE;
	EWorldChannel SelectedChannel = EWorldChannel::TUWorld_1;

public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ClickSound;

// 채널링
	UPROPERTY(meta = (BindWidget) , BlueprintReadWrite , Category = "Channel UI")
	class UComboBoxString* ChannelComboBox;

	UFUNCTION()
	void OnChannelChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

};
