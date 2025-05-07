// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GPChatMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPChatMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// �޽��� �ؽ�Ʈ�� ����
	void SetChatMessage(const FString& UserName, const FString& Message);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* UserNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;
	
};
