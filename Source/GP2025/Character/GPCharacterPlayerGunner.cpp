// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterPlayerGunner.h"

AGPCharacterPlayerGunner::AGPCharacterPlayerGunner()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/temporaryAssets/WomanPlayer/Woman.Woman'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animation/GunnerAnimation/ABP_Gunner.ABP_Gunner_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}
}
