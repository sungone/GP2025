#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPUserWidget.h"
#include "Interface/GPCharacterWidgetInterface.h"
#include "GPHpBarWidget.generated.h"

/**
 *¤¤
 */
UCLASS()
class GP2025_API UGPHpBarWidget : public UGPUserWidget, public IGPCharacterWidgetInterface
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
public:
	virtual void BindToCharacter(AGPCharacterBase* Character) override; 
	
	UFUNCTION()
	void UpdateHpBar(float HpRatio);
private:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;
};
