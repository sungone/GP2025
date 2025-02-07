// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPUserWidget.h"
#include "GPLevelWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPLevelWidget : public UUserWidget
{
	GENERATED_BODY()
public :
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void UpdateLevelText(int32 Level);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextLevel;
};
