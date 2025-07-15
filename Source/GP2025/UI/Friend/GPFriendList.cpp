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

void UGPFriendList::AddFriendEntry(uint32 Id , const FString& NickName, int32 Level , bool bOnline)
{
    if (!FriendEntryClass || !FriendScrollBox) return;

    UGPFriendEntry* EntryWidget = CreateWidget<UGPFriendEntry>(GetWorld(), FriendEntryClass);
    if (EntryWidget)
    {
        EntryWidget->SetFriendInfo(Id ,NickName, Level , bOnline);
        EntryWidget->OwnerBox = this->OwnerBox;
        FriendScrollBox->AddChild(EntryWidget);
    }
}

void UGPFriendList::RemoveFriendEntry(uint32 Id)
{
    if (!FriendScrollBox) return;

    TArray<UWidget*> Children = FriendScrollBox->GetAllChildren();

    for (UWidget* Child : Children)
    {
        UGPFriendEntry* Entry = Cast<UGPFriendEntry>(Child);
        if (Entry && Entry->FriendUserID == Id)
        {
            Entry->RemoveFromParent();
            UE_LOG(LogTemp, Log, TEXT("[FriendList] Removed Friend Entry: UserID=%d"), Id);
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[FriendList] No Friend Entry found for UserID=%d"), Id);
}
