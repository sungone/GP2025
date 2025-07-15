// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Friend/GPFriendList.h"
#include "Components/ScrollBox.h"
#include "UI/Friend/GPFriendEntry.h"


void UGPFriendList::NativeConstruct()
{
    Super::NativeConstruct();
}

void UGPFriendList::ClearFriendEntries()
{
    if (FriendScrollBox)
    {
        FriendScrollBox->ClearChildren();
    }
}

void UGPFriendList::AddFriendEntry(const FString& NickName, int32 Level)
{
    if (!FriendEntryClass || !FriendScrollBox) return;

    UGPFriendEntry* EntryWidget = CreateWidget<UGPFriendEntry>(GetWorld(), FriendEntryClass);
    if (EntryWidget)
    {
        EntryWidget->SetFriendInfo(NickName, Level);
        FriendScrollBox->AddChild(EntryWidget);
    }
}
