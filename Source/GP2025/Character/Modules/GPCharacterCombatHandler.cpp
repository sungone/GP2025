// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPCharacterCombatHandler.h"
#include "Character/GPCharacterBase.h"
#include "Character/GPCharacterMyplayer.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"


void UGPCharacterCombatHandler::Initialize(AGPCharacterBase* InOwner)
{
	Owner = InOwner;
}

void UGPCharacterCombatHandler::PlayAutoAttackMontage()
{
	if (!Owner || !AttackMontage) return;

	UAnimInstance* AnimInstance = Owner->GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || AnimInstance->Montage_IsPlaying(AttackMontage)) return;

	bIsAutoAttacking = true;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UGPCharacterCombatHandler::OnAutoAttackMontageEnded);
	AnimInstance->Montage_Play(AttackMontage, PlayRate);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);

	// End 델리게이트 실패 방지용 코드
	float MontageDuration = AttackMontage->GetPlayLength();
	float AdjustedDuration = MontageDuration / PlayRate;
	Owner->GetWorldTimerManager().SetTimer(
		AutoAttackFailSafeHandle,
		[this]()
		{
			if (bIsAutoAttacking)
			{
				UE_LOG(LogTemp, Error, TEXT("[Combat] Failsafe: Montage didn't end properly. Cleaning up manually."));
				bIsAutoAttacking = false;
				Owner->CharacterInfo.RemoveState(STATE_AUTOATTACK);

			}
		},
		AdjustedDuration + 0.2f,
		false
	);
}

void UGPCharacterCombatHandler::OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Owner || Montage != AttackMontage) return;
	bIsAutoAttacking = false;
	Owner->CharacterInfo.RemoveState(STATE_AUTOATTACK);

	// 실패 방지용 코드 타이머 제거
	if (Owner->GetWorldTimerManager().IsTimerActive(AutoAttackFailSafeHandle))
	{
		Owner->GetWorldTimerManager().ClearTimer(AutoAttackFailSafeHandle);
	}
}

void UGPCharacterCombatHandler::HandleDeath()
{
	if (!Owner) return;

	Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	Owner->SetActorEnableCollision(false);

	PlayDeadAnimation();

	FTimerHandle DeadTimerHandle;
	AGPCharacterBase* LocalOwner = Owner;

	if (AGPCharacterMyplayer* LocalMyPlayer = Cast<AGPCharacterMyplayer>(LocalOwner))
	{
		LocalMyPlayer->SetActorHiddenInGame(true);
		return;
	}
	
	Owner->GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([LocalOwner]()
		{
			if (LocalOwner)
			{
				LocalOwner->Destroy();
			}
		}), DeadEventDelayTime, false);
}

void UGPCharacterCombatHandler::PlayDeadAnimation()
{
	if (!Owner || !DeadMontage) return;

	UAnimInstance* AnimInstance = Owner->GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	AnimInstance->StopAllMontages(0.f);
	AnimInstance->Montage_Play(DeadMontage, 1.f);
}

void UGPCharacterCombatHandler::SetDeadEventDelay(float Delay)
{
	DeadEventDelayTime = Delay;
}

void UGPCharacterCombatHandler::PlayQSkillMontage()
{
	PlaySkillMontage(QSkillMontage);
}

void UGPCharacterCombatHandler::PlayESkillMontage()
{
	PlaySkillMontage(ESkillMontage);
}

void UGPCharacterCombatHandler::PlayRSkillMontage()
{
	PlaySkillMontage(RSkillMontage);
}

void UGPCharacterCombatHandler::PlaySkillMontage(UAnimMontage* SkillMontage)
{
	if (!Owner || !SkillMontage) return;

	UAnimInstance* AnimInstance = Owner->GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || AnimInstance->Montage_IsPlaying(SkillMontage)) return;

	bIsUsingSkill = true;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UGPCharacterCombatHandler::OnSkillMontageEnded);
	AnimInstance->Montage_Play(SkillMontage, PlayRate);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, SkillMontage);

	float AdjustedDuration = SkillMontage->GetPlayLength() / PlayRate;
	CurrentSkillMontage = SkillMontage;

	Owner->GetWorldTimerManager().SetTimer(
		SkillFailSafeHandle,
		[this]()
		{
			if (bIsUsingSkill)
			{
				UE_LOG(LogTemp, Error, TEXT("[Combat] Failsafe: Skill Montage did not end. Cleaning up manually."));
				bIsUsingSkill = false;

				// 상태 제거 (Q/E/R 중 어떤 것인지 확인 후 제거)
				if (CurrentSkillMontage == QSkillMontage)
					Owner->CharacterInfo.RemoveState(STATE_SKILL_Q);
				else if (CurrentSkillMontage == ESkillMontage)
					Owner->CharacterInfo.RemoveState(STATE_SKILL_E);
				else if (CurrentSkillMontage == RSkillMontage)
					Owner->CharacterInfo.RemoveState(STATE_SKILL_R);
			}
		},
		AdjustedDuration + 0.2f,
		false
	);
}

void UGPCharacterCombatHandler::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Owner)
		return;

	if (Montage == QSkillMontage || Montage == ESkillMontage || Montage == RSkillMontage)
	{
		bIsUsingSkill = false;

		if (Owner->CharacterInfo.HasState(STATE_SKILL_Q) && Montage == QSkillMontage)
			Owner->CharacterInfo.RemoveState(STATE_SKILL_Q);

		if (Owner->CharacterInfo.HasState(STATE_SKILL_E) && Montage == ESkillMontage)
			Owner->CharacterInfo.RemoveState(STATE_SKILL_E);

		if (Owner->CharacterInfo.HasState(STATE_SKILL_R) && Montage == RSkillMontage)
			Owner->CharacterInfo.RemoveState(STATE_SKILL_R);

		if (Owner->GetWorldTimerManager().IsTimerActive(SkillFailSafeHandle))
		{
			Owner->GetWorldTimerManager().ClearTimer(SkillFailSafeHandle);
		}
	}
}

void UGPCharacterCombatHandler::SetAttackMontage(UAnimMontage* Montage)
{
	AttackMontage = Montage;
}

void UGPCharacterCombatHandler::SetQSkillMontage(UAnimMontage* Montage)
{
	QSkillMontage = Montage;
}

void UGPCharacterCombatHandler::SetESkillMontage(UAnimMontage* Montage)
{
	ESkillMontage = Montage;
}

void UGPCharacterCombatHandler::SetRSkillMontage(UAnimMontage* Montage)
{
	RSkillMontage = Montage;
}

void UGPCharacterCombatHandler::SetDeadMontage(UAnimMontage* Montage)
{
	DeadMontage = Montage;
}
