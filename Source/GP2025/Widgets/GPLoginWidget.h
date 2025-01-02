// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "GPLoginWidget.generated.h"


/**
 * 
 */
UCLASS()
class GP2025_API UGPLoginWidget : public UUserWidget
{
	GENERATED_BODY()

//private :
//	void SendLoginDBPacket(bool isCA);
//	bool isCreate = false;
//	
//protected :
//	// virtual void NativeConstruct() override;
//	FString ID;
//	FString PW;
//
//public :
//	UPROPERTY(meta = (BindWidget))
//	UTextBlock* TextError;
//
//	UPROPERTY(meta = (BindWidget))
//	UTextBlock* TextCreateAccount;
//
//	UPROPERTY(meta = (BindWidget))
//	UTextBlock* TextLogin;
//
//	UPROPERTY(meta = (BindWidget))
//	UEditableTextBox* TBInputID;
//
//	UPROPERTY(meta = (BindWidget))
//	UEditableTextBox* TBInputPW;
//
//	UFUNCTION(BlueprintCallable)
//	void CreateAccount();
//
//	UFUNCTION(BlueprintCallable)
//	void CancleCreateAccount();
//
//	UFUNCTION()
//	void OnEntered(const FText& Text, ETextCommit::Type CommitMethod);
//
//	UFUNCTION(BlueprintCallable, Category = "UI")
//	void SetEnable(UWidget* Widget, bool b);
};
