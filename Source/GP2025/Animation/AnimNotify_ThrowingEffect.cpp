// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_ThrowingEffect.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

void UAnimNotify_ThrowingEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !ProjectileEffectClass) return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	FVector MuzzleLocation = MeshComp->GetSocketLocation(FName("WeaponSocket")) + MuzzleOffset;

	APlayerController* PC = Cast<APlayerController>(OwnerActor->GetInstigatorController());
	if (!PC) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * 10000.f);

	FHitResult Hit;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(ProjectileTrace), true, OwnerActor);

	FVector TargetPoint = TraceEnd;
	if (OwnerActor->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
	{
		TargetPoint = Hit.ImpactPoint;
	}

	FVector AdjustedFireDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();
	FRotator AdjustedFireRotation = AdjustedFireDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	OwnerActor->GetWorld()->SpawnActor<AActor>(
		ProjectileEffectClass,
		MuzzleLocation,
		AdjustedFireRotation,
		SpawnParams
	);
}
