// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "../../GP_Server/Source/Common/Common.h"
#include "GPChatMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPChatMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 메시지 텍스트를 설정
	void SetChatMessage(EChatChannel Channel, const FString& TargetName, const FString& Message);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ChannelText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TargetNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;
	
};
