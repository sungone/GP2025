// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPLoginWidget.generated.h"

class UTextBlock;
class UEditableTextBox;
class UButton;

/**
 * 
 */
UCLASS()
class GP2025_API UGPLoginWidget : public UUserWidget
{
	GENERATED_BODY()

protected :
	FTimerHandle HideErrorTimerHandle;
	FString ID_Str;
	FString PW_Str;

public :
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextError;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonLogin;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonSignUp;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonExit;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TBInputID;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TBInputPW;

	UFUNCTION()
	void OnEntered(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void OnLoginClicked();
	UFUNCTION()
	void OnSignUpClicked();
	UFUNCTION()
	void OnExitClicked();
	UFUNCTION()
	void HandleLoginFail(FString Message);

	void TryLogin();
	void TrySignUp();
	void ShowErrorMessage(const FString& Message, float Duration);
	void HideErrorMessage();
};
