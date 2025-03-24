// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"
#include "Interface/GPAnimationAttackInterface.h"
#include "Weapons/GPGun.h"
#include "Weapons/GPSword.h"
#include "Character/GPCharacterPlayer.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;

	// 캐릭터 타입 확인
	AGPCharacterPlayer* CharacterPlayer = Cast<AGPCharacterPlayer>(MeshComp->GetOwner());
	if (!CharacterPlayer) return;

	if (CharacterPlayer->WeaponActor)
	{
		if (Cast<AGPSword>(CharacterPlayer->WeaponActor))
		{
			IGPAnimationAttackInterface* AttackPawn = Cast<IGPAnimationAttackInterface>(CharacterPlayer);
			if (AttackPawn)
			{
				AttackPawn->AttackHitCheck();
			}
		}
		else if (Cast<AGPGun>(CharacterPlayer->WeaponActor))
		{
			Cast<AGPGun>(CharacterPlayer->WeaponActor)->FireBullet();
		}
	}
}
