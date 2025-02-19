// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GPSword.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Character/GPCharacterBase.h"

AGPSword::AGPSword()
{
	// 콜리전 캡슐 생성
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(WeaponMesh);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGPSword::OnWeaponOverlap);
}

void AGPSword::StartAttack()
{
	bIsAttacking = true;
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AGPSword::EndAttack()
{
	bIsAttacking = false;
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGPSword::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsAttacking && OtherActor && OtherActor != this)
	{
		AGPCharacterBase* HitCharacter = Cast<AGPCharacterBase>(OtherActor);
		if (HitCharacter)
		{
			UE_LOG(LogTemp, Log, TEXT("검이 %s에게 적중!"), *OtherActor->GetName());

			// 데미지 적용 (예제)
			// HitCharacter->TakeDamage(20.0f, FDamageEvent(), nullptr, this);
		}
	}
}
