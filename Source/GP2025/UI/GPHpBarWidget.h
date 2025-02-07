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
	UGPHpBarWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void UpdateHpBar(float HpRatio);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

};
