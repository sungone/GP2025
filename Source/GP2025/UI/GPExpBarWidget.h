// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPUserWidget.h"
#include "GPExpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPExpBarWidget : public UGPUserWidget
{
	GENERATED_BODY()
	
public :
	UGPExpBarWidget(const FObjectInitializer& ObjectInitializer);

protected :
	virtual void NativeConstruct() override;
public :
	FORCEINLINE void SetMaxExp(float NewMaxExp) { MaxExp = NewMaxExp; }
	void UpdateExpBar(float NewCurrentExp);
protected :
	UPROPERTY()
	TObjectPtr<class UProgressBar> ExpProgressBar;

	UPROPERTY()
	float MaxExp;
};
