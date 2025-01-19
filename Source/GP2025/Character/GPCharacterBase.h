// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../GP_Server/Proto.h"
#include "Interface/GPAnimationAttackInterface.h"
#include "GPCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	// �÷��̾�
	Warrior ,
	Gunner ,

	// ����
	Mouse
};

UCLASS()
class GP2025_API AGPCharacterBase : public ACharacter , public IGPAnimationAttackInterface
{
	GENERATED_BODY()

public:
	AGPCharacterBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// �������� ���� �ٸ� Ŭ���̾�Ʈ ������ ������Ʈ
	void SetClientInfoFromServer(FPlayerInfo& PlayerInfo_);

public:
	FPlayerInfo PlayerInfo;

// �⺻ ���� �ִϸ��̼� �� ���� �ִϸ��̼� ��Ÿ�� �ڵ�
public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TObjectPtr<class UAnimMontage> AutoAttackActionMontage;

	void ProcessAutoAttackCommand();
	void OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	bool bIsAutoAttacking = false;

// Control Data ����
protected :
	virtual void SetCharacterControlData(const class UGPCharacterControlData* CharacterControlData);

	UPROPERTY(EditAnywhere, Category = "CharacterControl", Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UGPCharacterControlData*> CharacterControlManager;

// Attack Hit Section
protected :
	virtual void AttackHitCheck() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
