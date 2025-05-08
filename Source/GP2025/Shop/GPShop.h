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

	UPROPERTY()
	class AGPCharacterMyplayer* MyPlayer;

	UFUNCTION()
	void OnShopExit();

	UFUNCTION()
	void UpdateMoneyText(int32 PlayerMoney);

	void SetMyPlayer(AGPCharacterMyplayer* Player);
	void SetOwningNPC(AGPCharacterNPC* NPC);

	UFUNCTION()
	void OnBuyItemClicked();

public:

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* ItemWrapBox;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<class UGPItemSlot> SlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	class UDataTable* ItemDataTable;

	/** 상점 아이템 슬롯을 생성하는 함수 */
	void PopulateShopItems();
};
