// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/GPCharacterWidgetInterface.h"
#include "GPUserNameWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPUserNameWidget : public UUserWidget, public IGPCharacterWidgetInterface
{
	GENERATED_BODY()
	

protected:
	virtual void NativeConstruct() override;
public:
	virtual void BindToCharacter(AGPCharacterBase* Character) override;

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NickNameText;

	UFUNCTION()
	void UpdateNickNameText(FString NewName);
};
