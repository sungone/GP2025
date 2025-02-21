#include "UI/GPLevelWidget.h"
#include "Components/TextBlock.h"
#include "Character/GPCharacterPlayer.h"

void UGPLevelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    TextLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtLevel")));
    ensure(TextLevel);
}

void UGPLevelWidget::BindToCharacter(AGPCharacterBase* Character)
{
    if (Character)
    {
        Character->OnLevelChanged.RemoveDynamic(this, &UGPLevelWidget::UpdateLevelText);
        Character->OnLevelChanged.AddDynamic(this, &UGPLevelWidget::UpdateLevelText);
        UpdateLevelText(Character->CharacterInfo.GetLevel());
    }
}

void UGPLevelWidget::UpdateLevelText(int32 Level)
{
    if (TextLevel)
    {
        TextLevel->SetText(FText::FromString(FString::Printf(TEXT("%dLv"), Level)));
    }
}

