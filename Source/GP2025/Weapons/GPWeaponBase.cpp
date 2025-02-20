// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GPWeaponBase.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AGPWeaponBase::AGPWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
}

// Called when the game starts or when spawned
void AGPWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGPWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGPWeaponBase::StartAttack()
{
}

void AGPWeaponBase::EndAttack()
{
}

void AGPWeaponBase::SetWeaponMesh(UStaticMesh* NewWeaponMesh)
{
	if (NewWeaponMesh == nullptr)
		return;

	WeaponMesh->SetStaticMesh(NewWeaponMesh);
}

void AGPWeaponBase::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

