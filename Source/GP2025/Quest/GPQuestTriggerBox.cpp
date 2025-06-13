// Fill out your copyright notice in the Description page of Project Settings.

#include "GPQuestTriggerBox.h"
#include "Character/GPCharacterMyplayer.h"
#include "UI/GPFloatingQuestText.h"
#include "Network/GPNetworkManager.h"
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

		// Quest 분기 처리
		switch (TriggerQuestType)
		{
		case 2: // CH1_GO_TO_E_FIRST
			MyPlayer->NetMgr->SendMyCompleteQuest(QuestType::CH1_GO_TO_E_FIRST);
			QuestMessage = TEXT("E동이 잠겨있다. 경비아저씨를 찾아가서 여쭤봐야 할것 같다.");
		case 4: // CH1_GO_TO_BUNKER
			MyPlayer->NetMgr->SendMyCompleteQuest(QuestType::CH1_GO_TO_BUNKER);
			QuestMessage = TEXT("벙커에 도착했다. 벙커에 있는 몬스터를 잡고 열쇠를 찾아보자.");
			break;

			// 추후 확장 가능
		default:
			UE_LOG(LogTemp, Warning, TEXT("QuestTriggerBox: AssignedQuest not handled."));
			break;
		}

		// 퀘스트 알림 출력
		if (!QuestMessage.IsEmpty())
		{
			UWorld* World = GetWorld();
			if (World)
			{
				FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 200.f);
				FRotator SpawnRotation = FRotator::ZeroRotator;

				AGPFloatingQuestText* QuestTextActor = World->SpawnActor<AGPFloatingQuestText>(
					AGPFloatingQuestText::StaticClass(),
					SpawnLocation,
					SpawnRotation);

				if (QuestTextActor)
				{
					QuestTextActor->SetQuestMessage(QuestMessage);
				}
			}
		}
	}
}