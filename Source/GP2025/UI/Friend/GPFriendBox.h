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
	// Friend / Request ��ư
	UPROPERTY(meta = (BindWidget))
	class UButton* FriendListButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RequestListButton;

	// WidgetSwitcher
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* FriendWidgetSwitcher;

	// ����ó �ε��� ���
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

	// �ݹ�
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
	/** Friendâ ��/�ݱ� �ִϸ��̼� */
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* OpenAnim;

	/** �ִϸ��̼� ��� �Լ� */
	void PlayOpenAnimation(bool bReverse);
};
