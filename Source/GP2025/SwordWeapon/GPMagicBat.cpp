// Fill out your copyright notice in the Description page of Project Settings.

#include "SwordWeapon/GPMagicBat.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/GPCharacterBase.h"

// Sets default values
AGPMagicBat::AGPMagicBat()
{
	PrimaryActorTick.bCanEverTick = true;

	// Weapon mesh
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	// Bind overlap event
	//CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AGPWeapon::OnHit);
}

void AGPMagicBat::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetOwner()) // Avoid hitting the owner
	{
		// Apply damage to the hit actor
		//UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);

		UE_LOG(LogTemp, Log, TEXT("Weapon hit: %s, Damage: %f"), *OtherActor->GetName(), Damage);
	}
}

void AGPMagicBat::AttachToPlayer(AGPCharacterBase* PlayerCharacter, FName SocketName)
{
	if (PlayerCharacter)
	{
		// Attach the weapon to the player's mesh
		AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

		// Set the weapon's owner
		SetOwner(PlayerCharacter);
	}
}

