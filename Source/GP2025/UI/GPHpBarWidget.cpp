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
        // ��������Ʈ �ߺ� ���ε� ����
        Character->OnHpChanged.RemoveDynamic(this, &UGPHpBarWidget::UpdateHpBar);
        Character->OnHpChanged.AddDynamic(this, &UGPHpBarWidget::UpdateHpBar);
        float HpRatio = Character->CharacterInfo.GetHp() / Character->CharacterInfo.GetMaxHp();
        UpdateHpBar(HpRatio);
    }
}

void UGPHpBarWidget::UpdateHpBar(float HpRatio)
{
    if (HpProgressBar)
    {
        HpProgressBar->SetPercent(HpRatio);
    }
}