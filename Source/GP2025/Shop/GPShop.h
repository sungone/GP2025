// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/GPItemStruct.h"
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
	virtual void NativeDestruct() override;

public:

	UPROPERTY(meta = (BindWidget))
	class UButton* BuyButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SellButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BuyWidgetSwitchButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SellWidgetSwitchButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MoneyText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResultMessage;

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

	FTimerHandle HideResultMsgTimerHandle;

	UFUNCTION()
	void HandleBuyItemResult(bool bSuccess, uint32 CurrentGold, const FString& Message);
	void ShowResultMessage(const FString& Message, float Duration);

	void HideResultMessage();

	UFUNCTION()
	void OnSellItemClicked();

	UFUNCTION()
	void HandleSellItemResult(bool bSuccess, uint32 NewGold, const FString& Message);
public:

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* BuyWrapBox;


	UPROPERTY(meta = (BindWidget))
	class UWrapBox* SellWrapBox;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<class UGPItemSlot> SlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	class UDataTable* ItemDataTable;

	// 슬롯 정보를 저장할 포인터
	UPROPERTY()
	class UGPItemSlot* CurrentSlot;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	class UGPShopItemViewerSlot* ShopViewerSlot;

	void SetCurrentSlot(UGPItemSlot* InSlot);

	/** 상점 아이템 슬롯을 생성하는 함수 */
	void PopulateShopItems();

	/* 판매 슬롯창을 생성하는 함수 */
	void PopulateSellItems();

	// 어떤 상점인지 지정
	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	EShopType CurrentShopType = EShopType::NONE;

	void SetShopType(EShopType NewShopType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ClickSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* SellAndBuySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ErrorSound;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* OpenAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CloseAnim;

	void OpenShopAnimation();
	void CloseShopAnimation();

};
