// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_FThrowingEffect.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

void UAnimNotify_FThrowingEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetOwner() || !ProjectileEffectClass)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	UWorld* World = Owner->GetWorld();
	if (!World)
	{
		return;
	}

	FVector MuzzleLocation = MeshComp->GetSocketLocation(FName("WeaponSocket")) + MuzzleOffset;

	APlayerController* PC = Cast<APlayerController>(Owner->GetInstigatorController());
	if (!PC) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * 10000.f);

	FHitResult Hit;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(FThrowingEffectTrace), true, Owner);

	FVector TargetPoint = TraceEnd;
	if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
	{
		TargetPoint = Hit.ImpactPoint;
	}

	FVector CenterDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();
	FRotator CenterRotation = CenterDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < NumProjectiles; ++i)
	{
		int32 OffsetFromCenter = i - (NumProjectiles / 2);

		FRotator SpreadRotation = CenterRotation;
		SpreadRotation.Yaw += OffsetFromCenter * SpreadAngle;

		World->SpawnActor<AActor>(
			ProjectileEffectClass,
			MuzzleLocation,
			SpreadRotation,
			SpawnParams
		);
	}
}
