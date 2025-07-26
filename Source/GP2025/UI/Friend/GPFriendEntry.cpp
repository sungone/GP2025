// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Friend/GPFriendEntry.h"
#include "Components/TextBlock.h"
#include "UI/Friend/GPFriendBox.h"
#include "Components/Button.h"


void UGPFriendEntry::SetFriendInfo(uint32 Id , const FString& NickName, int32 Level, bool bOnline)
{
    if (NicknameText)
    {
        NicknameText->SetText(FText::FromString(NickName));
    }

    if (LevelText)
    {
        LevelText->SetText(FText::Format(FText::FromString(TEXT("LV.{0}")), FText::AsNumber(Level)));
    }

    FriendUserID = Id;
    bIsOnline = bOnline;
}

void UGPFriendEntry::OnEntryClicked()
{
    if (OwnerBox)
    {
        OwnerBox->SetSelectedFriend(this);
    }
}

void UGPFriendEntry::NativeConstruct()
{
    Super::NativeConstruct();

    if (EntryButton)
    {
        EntryButton->OnClicked.AddDynamic(this, &UGPFriendEntry::OnEntryClicked);
    }
}

void UGPFriendEntry::SetSelected(bool bSelected)
{
    bIsSelected = bSelected;
    UpdateSelectionState();
}

void UGPFriendEntry::UpdateSelectionState()
{
    if (!BackgroundBorder) return;

    if (bIsSelected)
    {
        BackgroundBorder->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.4f));
    }
    else
    {
        BackgroundBorder->SetBrushColor(FLinearColor(1.0f, 1.0f, 1.0f, 0.2f));
    }
}
