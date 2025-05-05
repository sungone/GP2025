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

void UGPQuestWidget::OnQuestAccepted()
{
	if (!OwningNPC) return;

	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (MyPlayer && MyPlayer->UIManager)
	{
		MyPlayer->UIManager->GetInGameWidget()->QuestListWidget->ShowQuestEntry(TEXT("TinoQuest"));
	}

	UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (NetMgr)
	{
		NetMgr->SendMyRequestQuest(QuestType::DefeatTinoboss); // 일단 티노보스 퀘스트 요청
	}

	OwningNPC->ExitInteraction(); 
}

void UGPQuestWidget::OnQuestExit()
{
	if (OwningNPC)
	{
		OwningNPC->ExitInteraction();
		RemoveFromParent();
	}
}
