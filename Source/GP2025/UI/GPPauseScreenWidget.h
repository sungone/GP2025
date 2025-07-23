// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPPauseScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPPauseScreenWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// ��ư ���ε�
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitGameButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	// �Լ�
	UFUNCTION()
	void OnQuitGameClicked();

	UFUNCTION()
	void OnBackClicked();
	
};
