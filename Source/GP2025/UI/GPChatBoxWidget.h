// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "GPChatBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPChatBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	// ä�� �Է�â
	UPROPERTY(meta = (BindWidget))
	UEditableText* SendMessageText;

	// ä�� ���� ��ư
	UPROPERTY(meta = (BindWidget))
	UButton* EnterButton;

	// ä�� �޽��� ǥ�� ����
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox;

	// ä�� �Է� �Ϸ� �� ó��
	UFUNCTION()
	void OnChatCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	// ��ư Ŭ�� �� ó��
	UFUNCTION()
	void OnEnterButtonClicked();

	// ���� ���� ó��
	void HandleSendMessage();

	// ä��â ���ͷ� ����
	void SetKeyboardFocusToInput();


public:
	void AddChatMessage(const FString& UserName, const FString& Message);

	UFUNCTION()
	void HandleChatReceived(const FString& Sender, const FString& Message);

public:
	UPROPERTY(EditAnywhere, Category = "Chat")
	TSubclassOf<class UGPChatMessageWidget> ChatMessageWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ClickSound;
};
