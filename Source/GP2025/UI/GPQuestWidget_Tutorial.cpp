// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPQuestWidget_Tutorial.h"
#include "Kismet/GameplayStatics.h"
#include "Network/GPNetworkManager.h"
#include "Components/TextBlock.h"  
#include "Character/GPCharacterMyplayer.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Sound/SoundBase.h"
#include "Components/Button.h"

void UGPQuestWidget_Tutorial::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 바인딩 교체
	if (QuestAcceptButton)
	{
		QuestAcceptButton->OnClicked.Clear(); // 부모의 바인딩 제거
		QuestAcceptButton->OnClicked.AddDynamic(this, &UGPQuestWidget_Tutorial::OnTutorialAccepted);
	}

	if (QuestExitButton)
	{
		QuestExitButton->OnClicked.Clear();
		QuestExitButton->OnClicked.AddDynamic(this, &UGPQuestWidget_Tutorial::OnTutorialSkipped);
	}
}

void UGPQuestWidget_Tutorial::OnTutorialAccepted()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}

	if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		if (MyPlayer->NetMgr)
		{
			MyPlayer->NetMgr->SendMyCompleteQuest(QuestType::TUT_START);
			UE_LOG(LogTemp, Warning, TEXT("[TutorialWidget] SendMyCompleteQuest(TUT_START)"));
		}

		if (APlayerController* PC = Cast<APlayerController>(MyPlayer->GetController()))
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}

	RemoveFromParent();
}

void UGPQuestWidget_Tutorial::OnTutorialSkipped()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}

	if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		if (MyPlayer->NetMgr)
		{
			MyPlayer->NetMgr->SendMyRejectQuest(QuestType::TUT_START);
			UE_LOG(LogTemp, Warning, TEXT("[TutorialWidget] SendMyRejectQuest(TUT_START)"));
		}

		if (APlayerController* PC = Cast<APlayerController>(MyPlayer->GetController()))
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
	}

	RemoveFromParent();
}