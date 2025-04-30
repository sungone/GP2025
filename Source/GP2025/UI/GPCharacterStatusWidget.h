// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/GPCharacterWidgetInterface.h"
#include "GPCharacterStatusWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPCharacterStatusWidget : public UUserWidget , public IGPCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UGPHpBarWidget* WBPHpBar;

	UPROPERTY(meta = (BindWidget))
	class UGPLevelWidget* WBPLevelText;

	UPROPERTY(meta = (BindWidget))
	class UGPUserNameWidget* WBPName;

public :
	virtual void BindToCharacter(AGPCharacterBase* Character) override;
};
