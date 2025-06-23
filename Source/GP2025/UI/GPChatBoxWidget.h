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
	// 채팅 입력창
	UPROPERTY(meta = (BindWidget))
	UEditableText* SendMessageText;

	// 채팅 전송 버튼
	UPROPERTY(meta = (BindWidget))
	UButton* EnterButton;

	// 채팅 메시지 표시 영역
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox;

	// 채팅 입력 완료 시 처리
	UFUNCTION()
	void OnChatCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	// 버튼 클릭 시 처리
	UFUNCTION()
	void OnEnterButtonClicked();

	// 내부 전송 처리
	void HandleSendMessage();

	// 채팅창 엔터로 열기
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
