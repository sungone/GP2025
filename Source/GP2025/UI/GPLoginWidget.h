// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPLoginWidget.generated.h"

class UTextBlock;
class UEditableTextBox;
class UButton;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class GP2025_API UGPLoginWidget : public UUserWidget
{
	GENERATED_BODY()

protected :
	FTimerHandle HideErrorTimerHandle;

public :
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* LoginSwitcher;

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

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSignUpError;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonSignUpCencle;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonSignUpOK;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TBSignUpID;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TBSignUpPW;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TBSignUpNname;

	UFUNCTION()
	void SwitchWidget();
	UFUNCTION()
	void OnEntered(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION()
	void OnExitClicked();
	UFUNCTION()
	void HandleLoginFail(FString Message);

	UFUNCTION()
	void TryLogin();
	UFUNCTION()
	void TrySignUp();

	void ShowLoginErrorMessage(const FString& Message, float Duration);
	void ShowSignUpErrorMessage(const FString& Message, float Duration);
	void HideErrorMessage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ClickSound;
};
