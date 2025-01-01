// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterPlayerWarrior.h"

AGPCharacterPlayerWarrior::AGPCharacterPlayerWarrior()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/temporaryAssets/ManPlayer/Man.Man'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animation/WarriorAnimation/ABP_Warrior.ABP_Warrior"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AutoAttackMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/WarriorAnimation/AM_WarriorAttack.AM_WarriorAttack'"));
	if (AutoAttackMontageRef.Object)
	{
		AutoAttackActionMontage = AutoAttackMontageRef.Object;
	}
}
