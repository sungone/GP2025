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
	
private :
	void SendLoginDBPacket(bool isCA);
	bool isCreate = false;

protected :

	FString ID_Str;
	FString PW_Str;

public :
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextError;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextCreateAccount;



	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextLogin;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextExit;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonLogin;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonCreateAccount;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonExit;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TBInputID;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* TBInputPW;

	UFUNCTION(BlueprintCallable)
	void CreateAccount();

	UFUNCTION(BlueprintCallable)
	void CancleCreateAccount();

	UFUNCTION()
	void OnEntered(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetEnable(UWidget* widget, bool b);

	UFUNCTION()
	void OnLoginClicked();

	UFUNCTION()
	void OnExitClicked();
};
