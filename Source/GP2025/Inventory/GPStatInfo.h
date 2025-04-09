// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPStatInfo.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPStatInfo : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CrtRateText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CrtValueText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MaxHpText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DodgeText;

public :
	virtual void NativeConstruct() override;
public:
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void UpdateStatInfo();
};
