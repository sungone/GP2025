// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GPGun.h"
#include "GameFramework/Actor.h"
#include "Character/GPCharacterBase.h"
#include "Kismet/GameplayStatics.h"

AGPGun::AGPGun()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGPGun::StartAttack()
{
	FireBullet();
}

void AGPGun::FireBullet()
{
	// �Ѿ� �߻�
	FVector StartLocation = GetActorLocation();
	FVector ForwardVector = GetActorForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * 5000.f);

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
	{
		AGPCharacterBase* HitCharacter = Cast<AGPCharacterBase>(HitResult.GetActor());
		if (HitCharacter)
		{
			UE_LOG(LogTemp, Log, TEXT("�Ѿ��� %s���� ����!"), *HitResult.GetActor()->GetName());

			// ������ ���� (����)
			// HitCharacter->TakeDamage(10.0f, FDamageEvent(), nullptr, this);
		}
	}
}
