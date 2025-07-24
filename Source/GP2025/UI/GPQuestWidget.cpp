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
#include "Inventory/GPInventory.h"
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

void UGPQuestWidget::SetQuestDescription(const FText& Description)
{
	if (QuestDescriptionText)
	{
		QuestDescriptionText->SetText(Description);
	}
}

void UGPQuestWidget::SetQuestTitle(const FText& Title)
{
	if (QuestTypeText)
	{
		QuestTypeText->SetText(Title);
	}
}

void UGPQuestWidget::OnQuestAccepted()
{
	// 플레이어 접근
	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!MyPlayer) return;

	UGPNetworkManager* NetMgr = MyPlayer->NetMgr;
	if (!NetMgr) return;

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}

	if (!OwningNPC) // MainQuestStart
	{
		if (MyPlayer->CharacterInfo.GetCurrentQuest().Type == QuestType::TUT_START)
		{
			NetMgr->SendMyCompleteQuest();
			UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] MainQuestStart: SendMyCompleteQuest(TUT_COMPLETE)"));
		}

		RemoveFromParent();

		if (APlayerController* PC = Cast<APlayerController>(MyPlayer->GetController()))
		{
			if (MyPlayer->UIManager && MyPlayer->UIManager->bIsInventoryOpen)
			{
				PC->bShowMouseCursor = true;
				FInputModeGameAndUI InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
			}
			else
			{
				PC->bShowMouseCursor = true;
				PC->SetInputMode(FInputModeGameAndUI());
			}
		}

		return;
	}

	// NPC 타입에 따라 처리 분기
	switch (OwningNPC->NPCType)
	{
	case ENPCType::STUDENT:
	{
		NetMgr->SendMyCompleteQuest();
		UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] STUDENT NPC: SendMyCompleteQuest(CH1_TALK_TO_STUDENT_A)"));
		break;
	}
	case ENPCType::SECURITY:
	{
		NetMgr->SendMyCompleteQuest();
		UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] SECURITY NPC: SendMyCompleteQuest(CH1_FIND_JANITOR)"));
		break;
	}
	case ENPCType::PROFESSOR:
	{
		// 인벤토리 접근
		if (MyPlayer->UIManager && MyPlayer->UIManager->GetInventoryWidget())
		{
			UGPInventory* Inventory = MyPlayer->UIManager->GetInventoryWidget();

			if (Inventory->HasItemByType(51))  // 51번 아이템이 있는 경우에만 퀘스트 성공
			{
				NetMgr->SendMyCompleteQuest();
				UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] PROFESSOR NPC: Quest Complete Sent"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] PROFESSOR NPC: Missing Item 51 -> Cannot Complete Quest"));
				// 필요 시 여기서 UI 메시지 출력 가능 (예: "문서를 가지고 있지 않습니다.")
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[QuestWidget] Inventory Not Found!"));
		}
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
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}

	if (!OwningNPC) // MainQuestStart
	{
		AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (!MyPlayer) return;

		UGPNetworkManager* NetMgr = MyPlayer->NetMgr;
		if (!NetMgr) return;


		if (MyPlayer->CharacterInfo.GetCurrentQuest().Type == QuestType::TUT_START)
		{
			NetMgr->SendMyRejectQuest(QuestType::TUT_START);
			UE_LOG(LogTemp, Warning, TEXT("[QuestWidget] MainQuestStart: SendMyCompleteQuest(TUT_COMPLETE)"));
		}

		RemoveFromParent();

		if (APlayerController* PC = Cast<APlayerController>(MyPlayer->GetController()))
		{
			if (MyPlayer->UIManager && MyPlayer->UIManager->bIsInventoryOpen)
			{
				PC->bShowMouseCursor = true;
				FInputModeGameAndUI InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
			}
			else
			{
				PC->bShowMouseCursor = true;
				PC->SetInputMode(FInputModeGameAndUI());
			}
		}

		return;
	}


	if (OwningNPC)
	{
		OwningNPC->ExitInteraction();
	}
}

