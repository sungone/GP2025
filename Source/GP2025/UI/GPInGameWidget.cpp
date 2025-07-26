// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPInGameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Styling/SlateColor.h"
#include "GPInGameWidget.h"

void UGPInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (CurrentMapNameText)
	{
		CurrentMapNameText->SetText(FText::FromString(TEXT("한국공학대학교")));
	}
}

void UGPInGameWidget::UpdateHealthBar(float Ratio)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Ratio);
	}
}

void UGPInGameWidget::UpdateExpBar(float Ratio)
{
	if (ExpBar)
	{
		ExpBar->SetPercent(Ratio);
	}
}

void UGPInGameWidget::UpdateGold(int32 NewGold)
{
	if (MoneyText)
	{
		MoneyText->SetText(FText::AsNumber(NewGold));
	}
}

void UGPInGameWidget::UpdatePlayerLevel(int32_t NewLevel)
{
	if (LevelText)
	{
		LevelText->SetText(FText::AsNumber(NewLevel));

		if (LastLevel + 1 == NewLevel)
		{
			LevelUpAnimation();
			LastLevel = NewLevel;
		}
	}
}

FSlateColor UGPInGameWidget::GetQSkillTextColor()
{
	if (QSkillBar && QSkillBar->GetPercent() >= 1.0f - KINDA_SMALL_NUMBER)
	{
		return FSlateColor(FLinearColor(0.0f, 0.414f, 0.505f, 1.0f)); 
	}

	return FSlateColor(FLinearColor::White); // 기본 텍스트 색
}

FSlateColor UGPInGameWidget::GetESkillTextColor()
{
	if (ESkillBar && ESkillBar->GetPercent() >= 1.0f - KINDA_SMALL_NUMBER)
	{
		return FSlateColor(FLinearColor(0.0f, 0.414f, 0.505f, 1.0f));
	}

	return FSlateColor(FLinearColor::White);
}

FSlateColor UGPInGameWidget::GetRSkillTextColor()
{
	if (RSkillBar && RSkillBar->GetPercent() >= 1.0f - KINDA_SMALL_NUMBER)
	{
		return FSlateColor(FLinearColor(0.0f, 0.414f, 0.505f, 1.0f));
	}

	return FSlateColor(FLinearColor::White);
}

void UGPInGameWidget::SetCurrentMapName(const FString& MapName)
{
	if (CurrentMapNameText)
	{
		CurrentMapNameText->SetText(FText::FromString(MapName));
		UE_LOG(LogTemp, Log, TEXT("[InGameWidget] Updated current map name to: %s"), *MapName);
	}
}

void UGPInGameWidget::ShowGameMessage(const FText& Message, float Duration)
{
	if (!GameMessage) return;

	GameMessage->SetText(Message);
	GameMessageBox->SetVisibility(ESlateVisibility::Visible);

	PlayGameMessageFadeIn();

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			PlayGameMessageFadeOut();
			FTimerHandle HideHandle;
			GetWorld()->GetTimerManager().SetTimer(HideHandle, [this]()
				{
					GameMessageBox->SetVisibility(ESlateVisibility::Collapsed);
				}, 1.0f, false);

		}, Duration, false);
}

void UGPInGameWidget::LevelUpAnimation()
{
	PlayAnimation(LevelUpAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void UGPInGameWidget::HitByMonsterAnimation()
{
	PlayAnimation(HitByMonsterAnim, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void UGPInGameWidget::ShowLevelUpArrowTemporarily(UImage* ArrowImage, float Duration)
{
	if (!ArrowImage)
		return;

	// Visible 처리
	ArrowImage->SetVisibility(ESlateVisibility::Visible);

	if (LevelUpArrow)
	{
		PlayAnimation(LevelUpArrow);
	}

	// 일정 시간 뒤 Hidden 처리
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		[ArrowImage]()
		{
			if (ArrowImage)
			{
				ArrowImage->SetVisibility(ESlateVisibility::Hidden);
			}
		},
		Duration,
		false
	);
}

void UGPInGameWidget::ShowZoneChangeMessage(ZoneType NewZone)
{
	FText ZoneNameText;

	switch (NewZone)
	{
	case ZoneType::TIP:
		ZoneNameText = FText::FromString(TEXT("TIP"));
		break;
	case ZoneType::E:
		ZoneNameText = FText::FromString(TEXT("E동 2층"));
		break;
	case ZoneType::GYM:
		ZoneNameText = FText::FromString(TEXT("체육관"));
		break;
	case ZoneType::TUK:
		ZoneNameText = FText::FromString(TEXT("한국공학대학교"));
		break;
	case ZoneType::INDUSTY:
		ZoneNameText = FText::FromString(TEXT("산융 지하실"));
		break;
	default:
		ZoneNameText = FText::GetEmpty();
		break;
	}

	ShowGameMessage(ZoneNameText, 3.0f);
	SetCurrentMapName(ZoneNameText.ToString());
}

void UGPInGameWidget::PlayFadeOut()
{
	if (!FadeOverlay || !OverlayFadeOutAnim) return;

	// FadeOverlay를 보이게 설정
	FadeOverlay->SetVisibility(ESlateVisibility::Visible);

	// 애니메이션 0.5초 재생 (정상 속도)
	const float PlayRate = 1.0f;
	PlayAnimation(OverlayFadeOutAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, PlayRate);
}

void UGPInGameWidget::PlayFadeIn()
{
	if (!FadeOverlay || !OverlayFadeInAnim) return;

	// FadeOverlay를 보이게 설정
	FadeOverlay->SetVisibility(ESlateVisibility::Visible);

	// 애니메이션 0.5초 재생 (정상 속도)
	const float PlayRate = 1.0f;
	PlayAnimation(OverlayFadeInAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, PlayRate);
}

void UGPInGameWidget::PlayFade()
{
	if (!FadeOverlay || !OverlayFadeAnim) return;

	// FadeOverlay를 보이게 설정
	FadeOverlay->SetVisibility(ESlateVisibility::Visible);

	// 애니메이션 0.5초 재생 (정상 속도)
	const float PlayRate = 1.0f;
	PlayAnimation(OverlayFadeAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, PlayRate);
}

