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
        OwnerBox->SelectedFriendUserID = static_cast<uint32>(FriendUserID);
        UE_LOG(LogTemp, Log, TEXT("[FriendEntry] Selected FriendUserID: %d"), FriendUserID);
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