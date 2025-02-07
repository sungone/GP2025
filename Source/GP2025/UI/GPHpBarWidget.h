// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPUserWidget.h"
#include "GPHpBarWidget.generated.h"

/**
 *
 */
UCLASS()
class GP2025_API UGPHpBarWidget : public UGPUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdateHpBar(float HpRatio);

	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

};
