#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPUserWidget.h"
#include "Interface/GPCharacterWidgetInterface.h"
#include "GPExpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP2025_API UGPExpBarWidget : public UGPUserWidget, public IGPCharacterWidgetInterface
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
public:
	virtual void BindToCharacter(AGPCharacterBase* Character) override;

	UFUNCTION()
	void UpdateExpBar(float ExpRatio);
private:
	UPROPERTY()
	TObjectPtr<class UProgressBar> ExpProgressBar;
};
