// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "../../GP_Server/Source/Common/Common.h"
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
	UPROPERTY(meta = (BindWidget))
	UEditableText* SendMessageText;

	UPROPERTY(meta = (BindWidget))
	UButton* EnterButton;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox;

	UFUNCTION()
	void OnChatCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnEnterButtonClicked();

	void HandleSendMessage();
	bool HandleChatCommand(const FString& Input, class UGPNetworkManager* NetMgr);
	void ClearChatInput();
	void SetKeyboardFocusToInput();


public:
	void AddChatMessage(EChatChannel Channel, const FString& UserName, const FString& Message);

	UFUNCTION()
	void HandleChatReceived(uint8 Channel, const FString& Sender, const FString& Message);

public:
	UPROPERTY(EditAnywhere, Category = "Chat")
	TSubclassOf<class UGPChatMessageWidget> ChatMessageWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ClickSound;
};
