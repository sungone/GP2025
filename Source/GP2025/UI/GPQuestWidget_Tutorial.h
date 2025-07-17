// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GPQuestWidget.h"
#include "GPQuestWidget_Tutorial.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPQuestWidget_Tutorial : public UGPQuestWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnTutorialAccepted();

	UFUNCTION()
	void OnTutorialSkipped();
};
