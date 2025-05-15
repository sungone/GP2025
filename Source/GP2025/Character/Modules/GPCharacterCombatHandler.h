// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../GP_Server/Source/Common/Common.h"
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

	// ���
	void HandleDeath();
	void PlayDeadAnimation();
	void SetDeadMontage(UAnimMontage* Montage);
	void SetDeadEventDelay(float Delay);

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

	// <GunenrPlayer R Skill Ability>
	void ApplyAttackSpeedBoost(float BoostPlayRate, float Duration);
	void ResetAttackSpeed();
	float DefaultPlayRate = 2.0f;
	FTimerHandle AttackSpeedResetTimer;

	// <Warrior R Skill Ability>
private:
	int32 RemainingHits = 0;
	float HitInterval = 0.5f; 
	FTimerHandle MultiHitTimerHandle;

public:
	void PlayMultiHitSkill(int32 HitCount, float Interval);
	void ExecuteMultiHit();

	// <Warrior E Skill Ability>
private:
	FTimerHandle DashTimerHandle;
	bool bIsDashing = false;
	float DashDistance = 600.f; // ���� �Ÿ�
	float DashDuration = 1.f;   // ���� �ð�
	float DashElapsedTime = 0.0f;
	FVector DashStartLocation;
	FVector DashEndLocation;

public:
	void StartDash();
	void FinishDash();
	void UpdateDash();


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

	UPROPERTY()
	UAnimMontage* DeadMontage;

private :
	// AnimMontage PlayRate
	float PlayRate = 2.0f;
private:
	float DeadEventDelayTime = 0.5f;
	bool bIsAutoAttacking = false;
	bool bIsUsingSkill = false;

	// ��������Ʈ Montage End ó�� ���� ������
private :
	FTimerHandle AutoAttackFailSafeHandle;
	FTimerHandle SkillFailSafeHandle;

	UPROPERTY()
	UAnimMontage* CurrentSkillMontage = nullptr;
};
