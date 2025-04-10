// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPCharacterCombatHandler.h"
#include "Character/GPCharacterBase.h"
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
	bIsUsingSkill = false;
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
