// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GPCharacterCombatHandler.generated.h"

class AGPCharacterBase;
class UAnimMontage;

/**
 * 
 */
UCLASS()
class GP2025_API UGPCharacterCombatHandler : public UObject
{
	GENERATED_BODY()
	

public:
	void Initialize(AGPCharacterBase* InOwner);

	// �⺻ ����
	void PlayAutoAttackMontage();
	void OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// ��ų
	void PlayQSkillMontage();
	void PlayESkillMontage();
	void PlayRSkillMontage();
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// ���� ��ȸ (�ִϸ��̼� ���¸�)
	bool IsAutoAttacking() const { return bIsAutoAttacking; }
	bool IsUsingSkill() const { return bIsUsingSkill; }

	// �ִϸ��̼� ����
	void SetAttackMontage(UAnimMontage* Montage);
	void SetQSkillMontage(UAnimMontage* Montage);
	void SetESkillMontage(UAnimMontage* Montage);
	void SetRSkillMontage(UAnimMontage* Montage);

private:
	void PlaySkillMontage(UAnimMontage* SkillMontage);

public:
	UPROPERTY()
	AGPCharacterBase* Owner;

	UPROPERTY()
	UAnimMontage* AttackMontage;

	UPROPERTY()
	UAnimMontage* QSkillMontage;

	UPROPERTY()
	UAnimMontage* ESkillMontage;

	UPROPERTY()
	UAnimMontage* RSkillMontage;

private:
	bool bIsAutoAttacking = false;
	bool bIsUsingSkill = false;
};
