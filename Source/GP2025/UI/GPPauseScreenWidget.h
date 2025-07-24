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
	// 버튼 바인딩
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitGameButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	// 함수
	UFUNCTION()
	void OnQuitGameClicked();

	UFUNCTION()
	void OnBackClicked();
	
};
