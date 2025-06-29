// Fill out your copyright notice in the Description page of Project Settings.

#include "GPQuestTriggerBox.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "Inventory/GPInventory.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "UI/GPInGameWidget.h"
#include "Kismet/GameplayStatics.h"

AGPQuestTriggerBox::AGPQuestTriggerBox()
{
	OnActorBeginOverlap.AddDynamic(this, &AGPQuestTriggerBox::OnOverlapBegin);
}

void AGPQuestTriggerBox::BeginPlay()
{
	Super::BeginPlay();
}

void AGPQuestTriggerBox::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(OtherActor))
	{
		if (!MyPlayer->NetMgr)
			return;

		FString QuestMessage;
		UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget();
		if (!Inventory) return;

		UGPInGameWidget* InGameUI = MyPlayer->UIManager->GetInGameWidget();
		if (!InGameUI) return;

		// Quest 분기 처리
		switch (TriggerQuestType)
		{
		case 2: // CH1_GO_TO_E_FIRST
			if (!Inventory->HasItemByType(50)) // 열쇠 아이템을 가지고 있지 않으면
			{
				MyPlayer->NetMgr->SendMyCompleteQuest(QuestType::CH1_GO_TO_E_FIRST);
				QuestMessage = TEXT("E동이 잠겨있다. 경비아저씨를 찾아가서 여쭤봐야 할것 같다.");
				InGameUI->ShowGameMessage(QuestMessage, 3.f);
			}
			break;

		case 4: // CH1_GO_TO_BUNKER
			if (MyPlayer->CharacterInfo.GetCurrentQuest().QuestType == QuestType::CH1_GO_TO_BUNKER)
			{
				QuestMessage = TEXT("벙커에 도착했다. 벙커에 있는 몬스터를 잡고 열쇠를 찾아보자.");
				InGameUI->ShowGameMessage(QuestMessage, 3.f);
				MyPlayer->NetMgr->SendMyCompleteQuest(QuestType::CH1_GO_TO_BUNKER);
			}
			break;

			// 추후 확장 가능
		default:
			QuestMessage = TEXT("기본값.");
			UE_LOG(LogTemp, Warning, TEXT("QuestTriggerBox: AssignedQuest not handled."));
			break;
		}
	}
}