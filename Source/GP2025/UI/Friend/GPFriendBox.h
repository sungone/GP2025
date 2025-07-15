// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPFriendBox.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPFriendBox : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	// Friend / Request 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* FriendListButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RequestListButton;

	// WidgetSwitcher
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* FriendWidgetSwitcher;

	// 스위처 인덱스 상수
	enum WidgetIndex
	{
		FriendList = 0,
		RequestList = 1
	};

	UPROPERTY(meta = (BindWidget))
	class UUserWidget* FriendListWidget;

	UPROPERTY(meta = (BindWidget))
	class UButton* AddButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RemoveButton;

	// RequestList
	UPROPERTY(meta = (BindWidget))
	class UUserWidget* RequestedFriendWidget;

	UPROPERTY(meta = (BindWidget))
	class UButton* AcceptButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RejectButton;

	// 콜백
	UFUNCTION()
	void OnFriendListButtonClicked();

	UFUNCTION()
	void OnRequestListButtonClicked();

	UFUNCTION()
	void OnAddButtonClicked();

	UFUNCTION()
	void OnRemoveButtonClicked();

	UFUNCTION()
	void OnAcceptButtonClicked();

	UFUNCTION()
	void OnRejectButtonClicked();
	
public:
	// Getter (Optional)
	FORCEINLINE UUserWidget* GetFriendListWidget() const { return FriendListWidget; }
	FORCEINLINE UUserWidget* GetRequestedFriendWidget() const { return RequestedFriendWidget; }


public:
	/** Friend창 열/닫기 애니메이션 */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* OpenAnim;

	/** 애니메이션 재생 함수 */
	void PlayOpenAnimation(bool bReverse);
};
