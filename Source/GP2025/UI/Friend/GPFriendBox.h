// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
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
	class UGPFriendList* FriendListWidget;

	UPROPERTY(meta = (BindWidget))
	class UButton* AddButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RemoveButton;

	// RequestList
	UPROPERTY(meta = (BindWidget))
	class UGPFriendList* RequestedFriendWidget;

	UPROPERTY(meta = (BindWidget))
	class UButton* AcceptButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RejectButton;

	// �г��� �Է�â
	UPROPERTY(meta = (BindWidget))
	UEditableText* SendNicknameText;

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
	FORCEINLINE UGPFriendList* GetFriendListWidget() const { return FriendListWidget; }
	FORCEINLINE UGPFriendList* GetRequestedFriendWidget() const { return RequestedFriendWidget; }

	//void UpdateFriendList(const TArray<FFriendInfo>& FriendList);
	//void UpdateRequestList(const TArray<FFriendInfo>& RequestList);

public:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* OpenAnim;

	/** �ִϸ��̼� ��� �Լ� */
	void PlayOpenAnimation(bool bReverse);


	UPROPERTY()
	uint32 SelectedFriendUserID = MAX_uint32;


	void OnFriendAccepted(
		uint32 FriendUserID,
		const FString& Nickname,
		int32 Level,
		bool bAccepted,
		bool bOnline);
};
