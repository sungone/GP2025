// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPUserWidget.h"
#include "Interface/GPCharacterWidgetInterface.h"
#include "GPLevelWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPLevelWidget : public UUserWidget, public IGPCharacterWidgetInterface
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
public:
    virtual void BindToCharacter(AGPCharacterBase* Character) override;

    UFUNCTION()
    void UpdateLevelText(int32 Level);
    
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextLevel;
};
