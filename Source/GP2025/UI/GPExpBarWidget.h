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
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void UpdateExpBar(float ExpRatio);

	UPROPERTY()
	TObjectPtr<class UProgressBar> ExpProgressBar;
};
