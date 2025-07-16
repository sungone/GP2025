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
	UGPCharacterCombatHandler();
	void Initialize(AGPCharacterBase* InOwner);

	// 기본 공격
	void PlayAutoAttackMontage();
	void OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 사망
	void HandleDeath();
	void PlayDeadAnimation();
	void SetDeadMontage(UAnimMontage* Montage);
	void SetDeadEventDelay(float Delay);

	// 몬스터 Hit
	void PlayMonsterHitMontage();
	// 스킬
	void PlayQSkillMontage();
	void PlayESkillMontage();
	void PlayRSkillMontage();
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 상태 조회 (애니메이션 상태만)
	bool IsAutoAttacking() const { return bIsAutoAttacking; }
	bool IsUsingSkill() const { return bIsUsingSkill; }

	// 애니메이션 세팅
	void SetAttackWithoutWeaponMontage(UAnimMontage* Montage);
	void SetAttackMontage(UAnimMontage* Montage);
	void SetAttackMontageForWarrior(UAnimMontage* Montage1, UAnimMontage* Montage2, UAnimMontage* Montage3);
	void SetQSkillMontage(UAnimMontage* Montage);
	void SetESkillMontage(UAnimMontage* Montage);
	void SetRSkillMontage(UAnimMontage* Montage);
	void SetMonsterHitMontage(UAnimMontage* Montage);

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
	float DashDistance = 600.f; // 돌진 거리
	float DashDuration = 1.f;   // 돌진 시간
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
	UAnimMontage* AttackWithoutWeaponMontage;

	UPROPERTY()
	UAnimMontage* AttackMontage;

	UPROPERTY()
	UAnimMontage* WarriorAttackMontage_1;

	UPROPERTY()
	UAnimMontage* WarriorAttackMontage_2;

	UPROPERTY()
	UAnimMontage* QSkillMontage;

	UPROPERTY()
	UAnimMontage* ESkillMontage;

	UPROPERTY()
	UAnimMontage* RSkillMontage;

	UPROPERTY()
	UAnimMontage* DeadMontage;

	UPROPERTY()
	UAnimMontage* MonsterHitMontage;

private :
	// AnimMontage PlayRate
	float PlayRate = 2.0f;
private:
	float DeadEventDelayTime = 2.f;
	bool bIsAutoAttacking = false;
	bool bIsUsingSkill = false;

	// 델리게이트 Montage End 처리 실패 방지용
private :
	FTimerHandle AutoAttackFailSafeHandle;
	FTimerHandle SkillFailSafeHandle;

	UPROPERTY()
	UAnimMontage* CurrentSkillMontage = nullptr;

	// 몬스터가 가지는 사운드
public :
	UPROPERTY()
	USoundBase* BunkerMonsterAttackSound;

	UPROPERTY()
	USoundBase* EMonsterAttackSound;

	UPROPERTY()
	USoundBase* IndustryMonsterAttackSound;
};
