// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPQuestWidget.h"
#include "Components/TextBlock.h"  
#include "Components/Button.h"
#include "Character/GPCharacterNPC.h"
#include "kismet/GameplayStatics.h"
#include "UI/GPInGameWidget.h"
#include "Network/GPNetworkManager.h"
#include "UI/GPQuestListWidget.h"
#include "Character/GPCharacterMyplayer.h"
#include "Character/Modules/GPMyplayerUIManager.h"

void UGPQuestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (QuestAcceptButton)
	{
		QuestAcceptButton->OnClicked.AddDynamic(this, &UGPQuestWidget::OnQuestAccepted);
		QuestExitButton->OnClicked.AddDynamic(this, &UGPQuestWidget::OnQuestExit);
	}
}

void UGPQuestWidget::SetQuestDescription(const FString& Description)
{
	if (QuestDescriptionText)
	{
		QuestDescriptionText->SetText(FText::FromString(Description));
	}
}

void UGPQuestWidget::SetQuestTitle(const FString& Title)
{
	if (QuestTypeText)
	{
		QuestTypeText->SetText(FText::FromString(Title));
	}
}

void UGPQuestWidget::OnQuestAccepted()
{
	if (!OwningNPC) return;

	// 플레이어 접근
	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!MyPlayer) return;

	UGPNetworkManager* NetMgr = MyPlayer->NetMgr;
	if (!NetMgr) return;

	// NPC 타입에 따라 처리 분기
	switch (OwningNPC->NPCType)
	{
	case ENPCType::STUDENT:
	{
		// STUDENT 퀘스트 완료 요청 → 서버로 패킷 전송
		NetMgr->SendMyCompleteQuest(QuestType::CH1_TALK_TO_STUDENT_A);
		UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] STUDENT NPC: SendMyCompleteQuest(CH1_TALK_TO_STUDENT_A)"));
		break;
	}
	case ENPCType::SECURITY:
	{
		// STUDENT 퀘스트 완료 요청 → 서버로 패킷 전송
		NetMgr->SendMyCompleteQuest(QuestType::CH1_FIND_JANITOR);
		UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] SECURITY NPC: SendMyCompleteQuest(CH1_FIND_JANITOR)"));
		break;
	}
	case ENPCType::PROFESSOR:
	{
		// STUDENT 퀘스트 완료 요청 → 서버로 패킷 전송
		NetMgr->SendMyCompleteQuest(QuestType::CH1_RETURN_TO_TIP_WITH_DOC); // 일단 임시
		UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] SECURITY NPC: SendMyCompleteQuest(CH1_FIND_JANITOR)"));
		break;
	}
	default:
	{
		break;
	}
	}

	// UI 종료
	OwningNPC->ExitInteraction();
}

void UGPQuestWidget::OnQuestExit()
{
	if (OwningNPC)
	{
		OwningNPC->ExitInteraction();
	}
}
