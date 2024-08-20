// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/THAnimationAttackInterface.h"
#include "Interface/THCharacterWidgetInterface.h"
#include "THCharacterBase.generated.h"



UCLASS()
class TIGERHUNTER_API ATHCharacterBase : public ACharacter , public ITHAnimationAttackInterface , public ITHCharacterWidgetInterface
{
	GENERATED_BODY()

public:

	ATHCharacterBase();

	virtual void PostInitializeComponents() override;

// Combo Attack Action
protected :

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack , Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTHComboActionData> ComboActionData;

	void ProcessComboCommand();

	void ComboActionBegin();
	void ComboActionEnd(class UAnimMontage* TargetMontage , bool IsProperlyEnded);
	void SetComboCheckTimer();
	void ComboCheck();

	int32 CurrentCombo = 0; // Combo Value "0" : Combo is not started yet  1 or more : Combo is started
	FTimerHandle ComboTimerHandle;
	bool HasNextComboCommand = false;


// Attack Hit Check
protected :
	virtual void AttackHitCheck() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

// Dead Animation
protected :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;

	virtual void SetDead();
	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.f;

// Stat Section
protected :
	UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = Stat , Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTHCharacterStatComponent> Stat;

// UI Widget Section
protected :
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTHWidgetComponent> HpBar;

	virtual void SetupCharacterWidget(class UTHUserWidget* InUserWidget);
};
