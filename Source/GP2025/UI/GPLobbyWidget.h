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

	// ���� ĳ���� ���� ��ư
	UPROPERTY(meta = (BindWidget))
	class UGPCharacterSelectButtonWidget* SelectMan;

	// ���� ĳ���� ���� ��ư
	UPROPERTY(meta = (BindWidget))
	class UGPCharacterSelectButtonWidget* SelectWoman;

	UPROPERTY(meta = (BindWidget))
	class UButton* GameStartButton;

	UPROPERTY(meta = (BindWidget))
	UImage* PreviewImage;

	// ���� ĳ���� �̹��� ���
	UPROPERTY(EditAnywhere, Category = "Preview")
	FString ManImagePath;

	// ���� ĳ���� �̹��� ���
	UPROPERTY(EditAnywhere, Category = "Preview")
	FString WomanImagePath;

	// ĳ���� ���� ó��
	UFUNCTION()
	void OnCharacterSelected(uint8 NewType);

	/** �̹��� ���� �Լ� */
	void UpdatePreviewImage(const FString& ImagePath);

	// ���� ����
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

// ä�θ�
	UPROPERTY(meta = (BindWidget) , BlueprintReadWrite , Category = "Channel UI")
	class UComboBoxString* ChannelComboBox;

	UFUNCTION()
	void OnChannelChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

};
