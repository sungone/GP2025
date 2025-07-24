// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPChannelSelectWidget.generated.h"


class UComboBoxString;
class UButton;

/**
 * 
 */
UCLASS()
class GP2025_API UGPChannelSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnChannelSelected(FString SelectedItem, ESelectInfo::Type SelectionType);

public:
	UPROPERTY(meta = (BindWidget))
	UComboBoxString* ChannelComboBox;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UFUNCTION()
	void OnConfirmClicked();
	UFUNCTION()
	void UpdateChannelState();

protected:
	// ��ư ���ε� �Լ�
	UFUNCTION()
	void OnBackClicked();

	UFUNCTION()
	void OnQuitClicked();

public:
	// ���ư��� ��ư
	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	// ���� ���� ��ư
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	EWorldChannel SelectedChannel;
};
