// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPShop.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPShop : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

public :

	UPROPERTY(meta = (BindWidget))
	class UButton* BuyButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SellButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MoneyText;

	UPROPERTY()
	class AGPCharacterNPC* OwningNPC;

	UFUNCTION()
	void OnShopExit();
};
