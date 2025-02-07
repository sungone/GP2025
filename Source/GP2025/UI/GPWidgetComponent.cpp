#include "UI/GPWidgetComponent.h"
#include "GPUserWidget.h"
#include "Character/GPCharacterBase.h"
#include "Interface/GPCharacterWidgetInterface.h"

void UGPWidgetComponent::InitWidget()
{
    Super::InitWidget();

    UUserWidget* WidgetInstance = GetWidget();
    if (!WidgetInstance) return;

    IGPCharacterWidgetInterface* WidgetInterface = Cast<IGPCharacterWidgetInterface>(WidgetInstance);
    if (WidgetInterface)
    {
        AGPCharacterBase* Character = Cast<AGPCharacterBase>(GetOwner());
        if (Character)
        {
            WidgetInterface->BindToCharacter(Character);
        }
    }
}
