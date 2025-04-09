// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPUserNameWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPUserNameWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Nickname")
	class UTextBlock* NickNameText;

	UFUNCTION(BlueprintCallable, Category = "Nickname")
	void SetNickNameText(const FString& NewName);
};
