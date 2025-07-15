// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Friend/GPFriendBox.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UGPFriendBox::NativeConstruct()
{
	Super::NativeConstruct();

	// 탭 전환 버튼 바인딩
	if (FriendListButton)
	{
		FriendListButton->OnClicked.AddDynamic(this, &UGPFriendBox::OnFriendListButtonClicked);
	}

	if (RequestListButton)
	{
		RequestListButton->OnClicked.AddDynamic(this, &UGPFriendBox::OnRequestListButtonClicked);
	}

	// FriendList 버튼 바인딩
	if (AddButton)
	{
		AddButton->OnClicked.AddDynamic(this, &UGPFriendBox::OnAddButtonClicked);
	}

	if (RemoveButton)
	{
		RemoveButton->OnClicked.AddDynamic(this, &UGPFriendBox::OnRemoveButtonClicked);
	}

	// RequestList 버튼 바인딩
	if (AcceptButton)
	{
		AcceptButton->OnClicked.AddDynamic(this, &UGPFriendBox::OnAcceptButtonClicked);
	}

	if (RejectButton)
	{
		RejectButton->OnClicked.AddDynamic(this, &UGPFriendBox::OnRejectButtonClicked);
	}

	// 초기 탭은 FriendList
	if (FriendWidgetSwitcher)
	{
		FriendWidgetSwitcher->SetActiveWidgetIndex(WidgetIndex::FriendList);
		UE_LOG(LogTemp, Log, TEXT("[FriendBox] Initial tab set to FriendList."));
	}
}

void UGPFriendBox::OnFriendListButtonClicked()
{
	if (FriendWidgetSwitcher)
	{
		FriendWidgetSwitcher->SetActiveWidgetIndex(WidgetIndex::FriendList);
		UE_LOG(LogTemp, Log, TEXT("[FriendBox] Switched to FriendList tab."));
	}
}

void UGPFriendBox::OnRequestListButtonClicked()
{
	if (FriendWidgetSwitcher)
	{
		FriendWidgetSwitcher->SetActiveWidgetIndex(WidgetIndex::RequestList);
		UE_LOG(LogTemp, Log, TEXT("[FriendBox] Switched to RequestList tab."));
	}
}

void UGPFriendBox::OnAddButtonClicked()
{
	// TODO: NetMgr → SendFriendAddPacket
	UE_LOG(LogTemp, Log, TEXT("[FriendBox] AddButton clicked → Send FriendAddPacket to server."));
}

void UGPFriendBox::OnRemoveButtonClicked()
{
	// TODO: NetMgr → SendFriendRemovePacket
	UE_LOG(LogTemp, Log, TEXT("[FriendBox] RemoveButton clicked → Send FriendRemovePacket to server."));
}

void UGPFriendBox::OnAcceptButtonClicked()
{
	// TODO: NetMgr → SendFriendAcceptPacket
	UE_LOG(LogTemp, Log, TEXT("[FriendBox] AcceptButton clicked → Send FriendAcceptPacket to server."));
}

void UGPFriendBox::OnRejectButtonClicked()
{
	// TODO: NetMgr → SendFriendRejectPacket
	UE_LOG(LogTemp, Log, TEXT("[FriendBox] RejectButton clicked → Send FriendRejectPacket to server."));
}

void UGPFriendBox::UpdateFriendList(const TArray<FFriendInfo>& FriendList)
{

}

void UGPFriendBox::UpdateRequestList(const TArray<FFriendInfo>& RequestList)
{

}

void UGPFriendBox::PlayOpenAnimation(bool bReverse)
{
	if (!OpenAnim) return;

	PlayAnimation(
		OpenAnim,
		0.f,
		1,
		bReverse ? EUMGSequencePlayMode::Reverse : EUMGSequencePlayMode::Forward,
		1.f
	);
}
