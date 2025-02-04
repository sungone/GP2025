// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPLevelWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPLevelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public :
	void SetLevelText(int32 Level);

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextLevel;
};
