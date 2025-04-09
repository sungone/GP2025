// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPUserNameWidget.h"
#include "Components/TextBlock.h"
#include "Character/GPCharacterPlayer.h"

void UGPUserNameWidget::NativeConstruct()
{
    Super::NativeConstruct();

    NickNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtLevel")));
}

void UGPUserNameWidget::BindToCharacter(AGPCharacterBase* Character)
{
    if (Character)
    {
        Character->OnNickNameChanged.RemoveDynamic(this, &UGPUserNameWidget::UpdateNickNameText);
        Character->OnNickNameChanged.AddDynamic(this, &UGPUserNameWidget::UpdateNickNameText);
        UpdateNickNameText(Character->CharacterInfo.GetName());
    }
}

void UGPUserNameWidget::UpdateNickNameText(FString NewName)
{
    if (NickNameText)
    {
        NickNameText->SetText(FText::FromString(NewName));
    }
}
   
