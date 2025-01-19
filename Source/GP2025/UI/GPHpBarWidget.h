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
public :
	UGPHpBarWidget(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }
	void UpdateHpBar(float NewCurrentHp);

protected :
	virtual void NativeConstruct() override;

protected :
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
