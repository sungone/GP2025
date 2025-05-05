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

	AnimInstance->Montage_Play(AttackMontage, 2.f);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackMontage);
}

void UGPCharacterCombatHandler::OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Owner || Montage != AttackMontage) return;
	bIsAutoAttacking = false;
	Owner->CharacterInfo.RemoveState(STATE_AUTOATTACK);

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
	AnimInstance->Montage_Play(SkillMontage, 2.f);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, SkillMontage);
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
