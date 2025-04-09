// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPUserNameWidget.h"
#include "Components/TextBlock.h"

void UGPUserNameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (NickNameText)
	{
		NickNameText->SetText(FText::FromString(TEXT("Player")));
	}
}

void UGPUserNameWidget::SetNickNameText(const FString& NewName)
{
	if (NickNameText)
	{
		NickNameText->SetText(FText::FromString(NewName));
	}
}