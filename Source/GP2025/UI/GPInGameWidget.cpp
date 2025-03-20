// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPInGameWidget.h"
#include "Character/GPCharacterMyplayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
//
//void UGPInGameWidget::NativeConstruct()
//{
//	Super::NativeConstruct();
//
//	MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//
//    if (MyPlayer)
//    {
//        // 초기 UI 값 설정
//        UpdateHealthBar(MyPlayer->CharacterInfo.Stats.MaxHp / MyPlayer->CharacterInfo.Stats.Hp);
//        UpdateExpBar(MyPlayer->CharacterInfo.Stats.MaxExp / MyPlayer->CharacterInfo.Stats.Exp);
//        UpdatePlayerLevel(MyPlayer->CharacterInfo.Stats.Level);
//
//        // 플레이어의 델리게이트와 UI 함수 연결
//        MyPlayer->OnHpChanged.AddDynamic(this, &UGPInGameWidget::UpdateHealthBar);
//        MyPlayer->OnExpChanged.AddDynamic(this, &UGPInGameWidget::UpdateExpBar);
//        MyPlayer->OnLevelChanged.AddDynamic(this, &UGPInGameWidget::UpdatePlayerLevel);
//    }
//}
//
//void UGPInGameWidget::UpdateHealthBar(float Ratio)
//{
//    if (HealthBar)
//    {
//        HealthBar->SetPercent(Ratio);
//    }
//}
//
//void UGPInGameWidget::UpdateExpBar(float Ratio)
//{
//    if (ExpBar)
//    {
//        ExpBar->SetPercent(Ratio);
//    }
//}
//
//void UGPInGameWidget::UpdatePlayerLevel(int32 NewLevel)
//{
//    if (PlayerLevelText)
//    {
//        PlayerLevelText->SetText(FText::AsNumber(NewLevel));
//    }
//}

void UGPInGameWidget::NativeConstruct()
{
	HpPercent = 0.3;
	ExpPercent = 0.8;
	LevelText = TEXT("1Lv");
}
