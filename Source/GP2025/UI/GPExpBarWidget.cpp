#include "UI/GPExpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Character/GPCharacterBase.h"
#include "Interface/GPCharacterWidgetInterface.h"

void UGPExpBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ExpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbExpBar")));
    ensure(ExpProgressBar);
}

void UGPExpBarWidget::BindToCharacter(AGPCharacterBase* Character)
{
    if (Character)
    {
        Character->OnExpChanged.RemoveDynamic(this, &UGPExpBarWidget::UpdateExpBar);
        Character->OnExpChanged.AddDynamic(this, &UGPExpBarWidget::UpdateExpBar);
        float ExpRatio = Character->CharacterInfo.GetExp() / Character->CharacterInfo.GetMaxExp();
        UpdateExpBar(ExpRatio);
    }
}

void UGPExpBarWidget::UpdateExpBar(float ExpRatio)
{
    if (ExpProgressBar)
    {
        ExpProgressBar->SetPercent(ExpRatio);
    }
}
