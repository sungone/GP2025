#include "UI/GPHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Character/GPCharacterBase.h"
#include "Interface/GPCharacterWidgetInterface.h"

void UGPHpBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
    ensure(HpProgressBar);
}

void UGPHpBarWidget::BindToCharacter(AGPCharacterBase* Character)
{
    if (Character)
    {
        // 델리게이트 중복 바인딩 방지
        Character->OnHpChanged.RemoveDynamic(this, &UGPHpBarWidget::UpdateHpBar);
        Character->OnHpChanged.AddDynamic(this, &UGPHpBarWidget::UpdateHpBar);
    }
}

void UGPHpBarWidget::UpdateHpBar(float HpRatio)
{
    if (HpProgressBar)
    {
        HpProgressBar->SetPercent(HpRatio);
    }
}