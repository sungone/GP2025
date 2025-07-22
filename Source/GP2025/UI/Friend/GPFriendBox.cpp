// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Friend/GPFriendBox.h"
#include "Components/Button.h"
#include "UI/Friend/GPFriendList.h"
#include "UI/Friend/GPFriendEntry.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "kismet/GameplayStatics.h"
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

	if (FriendListWidget)
	{
		FriendListWidget->OwnerBox = this;
	}

	if (RequestedFriendWidget)
	{
		RequestedFriendWidget->OwnerBox = this;
	}
}

void UGPFriendBox::OnFriendListButtonClicked()
{
	if (FriendWidgetSwitcher)
	{
		FriendWidgetSwitcher->SetActiveWidgetIndex(WidgetIndex::FriendList);
	}
}

void UGPFriendBox::OnRequestListButtonClicked()
{
	if (FriendWidgetSwitcher)
	{
		FriendWidgetSwitcher->SetActiveWidgetIndex(WidgetIndex::RequestList);
	}
}

void UGPFriendBox::OnAddButtonClicked()
{
	if (!SendNicknameText) return;

	FText InputText = SendNicknameText->GetText();
	FString TargetNickname = InputText.ToString();

	if (TargetNickname.IsEmpty())
	{
		return;
	}

	if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		if (MyPlayer->NetMgr)
		{
			MyPlayer->NetMgr->SendMyFriendRequest(TargetNickname);
		}
	}

	SendNicknameText->SetText(FText::GetEmpty());
}

void UGPFriendBox::OnRemoveButtonClicked()
{

	if (SelectedFriendUserID == MAX_uint32)
	{
		return;
	}

	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (MyPlayer && MyPlayer->NetMgr)
	{
		MyPlayer->NetMgr->SendMyFriendRemove(SelectedFriendUserID);
	}

	if (FriendListWidget)
	{
		UGPFriendList* FriendList = Cast<UGPFriendList>(FriendListWidget);
		if (FriendList)
		{
			FriendList->RemoveFriendEntry(SelectedFriendUserID);
		}
	}

	// 선택 초기화
	SelectedFriendUserID = -1;
}

void UGPFriendBox::OnAcceptButtonClicked()
{
	if (SelectedFriendUserID == MAX_uint32)
	{
		return;
	}


	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (MyPlayer && MyPlayer->NetMgr)
	{
		MyPlayer->NetMgr->SendMyFriendAccept(SelectedFriendUserID);
	}

	if (RequestedFriendWidget)
	{
		UGPFriendList* RequestList = Cast<UGPFriendList>(RequestedFriendWidget);
		if (RequestList)
		{
			RequestList->RemoveFriendEntry(SelectedFriendUserID);
		}
	}
}

void UGPFriendBox::OnRejectButtonClicked()
{
	if (SelectedFriendUserID == MAX_uint32)
	{
		return;
	}

	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (MyPlayer && MyPlayer->NetMgr)
	{
		MyPlayer->NetMgr->SendMyFriendReject(SelectedFriendUserID);
	}

	if (RequestedFriendWidget)
	{
		UGPFriendList* RequestList = Cast<UGPFriendList>(RequestedFriendWidget);
		if (RequestList)
		{
			RequestList->RemoveFriendEntry(SelectedFriendUserID);
		}
	}

	SelectedFriendUserID = -1;
}

void UGPFriendBox::PlayOpenAnimation()
{
	if (!OpenAnim_Re) return;

	PlayAnimation(OpenAnim_Re, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void UGPFriendBox::PlayCloseAnimation()
{
	if (!OpenAnim_Re) return;

	PlayAnimation(OpenAnim_Re, 0.f, 1, EUMGSequencePlayMode::Reverse, 1.f);

}

void UGPFriendBox::AddToRequestedList(uint32 FriendUserID, const FString& Nickname, int32 Level, bool bOnline)
{
	if (FriendListWidget)
	{
		RequestedFriendWidget->AddFriendEntry(FriendUserID, Nickname, Level, bOnline);
	}
}

void UGPFriendBox::RemoveFromRequestedList(uint32 FriendUserID)
{
	if (RequestedFriendWidget)
	{
		RequestedFriendWidget->RemoveFriendEntry(FriendUserID);
	}
}

void UGPFriendBox::AddToFriendList(uint32 FriendUserID, const FString& Nickname, int32 Level, bool bOnline)
{
	if (FriendListWidget)
	{
		FriendListWidget->RemoveFriendEntry(FriendUserID);
		FriendListWidget->AddFriendEntry(FriendUserID, Nickname, Level, bOnline);
	}
}

void UGPFriendBox::RemoveFromFriendList(uint32 FriendUserID)
{
	if (FriendListWidget)
	{
		FriendListWidget->RemoveFriendEntry(FriendUserID);
	}
}
