// Fill out your copyright notice in the Description page of Project Settings.

#include "GPQuestTriggerBox.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"
#include "Inventory/GPInventory.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "UI/GPInGameWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

AGPQuestTriggerBox::AGPQuestTriggerBox()
{
	OnActorBeginOverlap.AddDynamic(this, &AGPQuestTriggerBox::OnOverlapBegin);

	static ConstructorHelpers::FObjectFinder<UDataTable> QuestMessageTableObj(
		TEXT("/Game/Quest/GPQuestMessageTable.GPQuestMessageTable")
	);

	if (QuestMessageTableObj.Succeeded())
	{
		QuestMessageTable = QuestMessageTableObj.Object;
	}
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

		UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget();
		if (!Inventory) return;

		UGPInGameWidget* InGameUI = MyPlayer->UIManager->GetInGameWidget();
		if (!InGameUI) return;

		FText QuestMessage = FText::FromString(TEXT("퀘스트 메시지를 찾지 못했습니다."));

		if (QuestMessageTable)
		{
			// RowName은 QuestID와 매칭 (TriggerQuestType과 같은 값 사용)
			const FName RowName = FName(*FString::FromInt(TriggerQuestType));
			FGPQuestMessageStruct* Row = QuestMessageTable->FindRow<FGPQuestMessageStruct>(RowName, TEXT("QuestMessageLookup"));

			if (Row)
			{
				QuestMessage = Row->QuestMessage;
			}
		}

		// Quest 분기 처리
		switch (TriggerQuestType)
		{
		case 30: // CH1_GO_TO_E_FIRST
			if (MyPlayer->CharacterInfo.GetCurrentQuest().QuestType == QuestType::CH1_GO_TO_E_FIRST)
			{
				if (!Inventory->HasItemByType(50)) // 열쇠 아이템을 가지고 있지 않으면
				{
					MyPlayer->NetMgr->SendMyCompleteQuest();
					InGameUI->ShowGameMessage(QuestMessage, 3.f);
				}
			}
			break;

		case 31: // CH1_GO_TO_BUNKER
			if (MyPlayer->CharacterInfo.GetCurrentQuest().Type == QuestType::CH1_GO_TO_BUNKER)
			{
				InGameUI->ShowGameMessage(QuestMessage, 3.f);
				MyPlayer->NetMgr->SendMyCompleteQuest();
			}
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("QuestTriggerBox: AssignedQuest not handled."));
			break;
		}
	}
}