// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPStatInfo.h"
#include "Components/TextBlock.h"
#include "Character/GPCharacterMyplayer.h" 
#include "Kismet/GameplayStatics.h"

void UGPStatInfo::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateStatInfo();
}

void UGPStatInfo::UpdateStatInfo()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(PC->GetPawn()))
		{
			const FStatData& Stat = MyPlayer->CharacterInfo.Stats;

			if (DamageText)
				DamageText->SetText(FText::AsNumber(Stat.Damage));

			if (CrtRateText)
				CrtRateText->SetText(FText::FromString(FString::Printf(TEXT("%.0f%%"), Stat.CrtRate * 100)));

			if (CrtValueText)
				CrtValueText->SetText(FText::FromString(FString::Printf(TEXT("x%.1f"), Stat.CrtValue)));

			if (MaxHpText)
				MaxHpText->SetText(FText::AsNumber(Stat.MaxHp));

			if (DodgeText)
				DodgeText->SetText(FText::FromString(FString::Printf(TEXT("%.0f%%"), Stat.Dodge * 100)));
		}
	}
}
