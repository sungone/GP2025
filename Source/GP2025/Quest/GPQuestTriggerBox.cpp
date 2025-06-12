// Fill out your copyright notice in the Description page of Project Settings.

#include "GPQuestTriggerBox.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"

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

		// Quest 분기 처리
		switch (TriggerQuestType)
		{
		case 2: // CH1_GO_TO_E_FIRST
			MyPlayer->NetMgr->SendMyCompleteQuest(QuestType::CH1_GO_TO_E_FIRST);

		case 4: // CH1_GO_TO_BUNKER
			MyPlayer->NetMgr->SendMyCompleteQuest(QuestType::CH1_GO_TO_BUNKER);
			break;

			// 추후 확장 가능
		default:
			UE_LOG(LogTemp, Warning, TEXT("QuestTriggerBox: AssignedQuest not handled."));
			break;
		}
	}
}