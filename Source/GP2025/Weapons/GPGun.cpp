// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/GPGun.h"
#include "GameFramework/Actor.h"
#include "Character/GPCharacterPlayer.h"
#include "Network/GPNetworkManager.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Physics/GPCollision.h"

AGPGun::AGPGun()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGPGun::AttackHitCheck()
{
    AGPCharacterPlayer* PlayerCharacter = Cast<AGPCharacterPlayer>(GetOwner());
    if (!PlayerCharacter) return;

    USkeletalMeshComponent* BodyMesh = PlayerCharacter->GetCharacterMesh(); 
    if (!BodyMesh) return;

	const FTransform SocketTransform = BodyMesh->GetSocketTransform(TEXT("WeaponSocket"));
	FVector MuzzleLocation = SocketTransform.GetLocation();

    APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
    if (!PC) return;

    FVector CamLocation;
    FRotator CamRotation;
    PC->GetPlayerViewPoint(CamLocation, CamRotation); 

    FVector TraceStart = CamLocation;
    FVector TraceEnd = TraceStart + (CamRotation.Vector() * ValidRange);

    FHitResult CameraHit;
    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(this);
    TraceParams.AddIgnoredActor(GetOwner());

    bool bCameraHit = GetWorld()->LineTraceSingleByChannel(
        CameraHit, TraceStart, TraceEnd, CCHANNEL_GPACTION, TraceParams);

    FVector AimTargetLocation = bCameraHit ? CameraHit.ImpactPoint : TraceEnd;

    FVector ShotDirection = (AimTargetLocation - MuzzleLocation).GetSafeNormal();
    FVector FinalTraceEnd = MuzzleLocation + ShotDirection * ValidRange;

    FHitResult FinalHit;
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        FinalHit, MuzzleLocation, FinalTraceEnd, CCHANNEL_GPACTION, TraceParams);

    DrawDebugLine(GetWorld(), MuzzleLocation, FinalTraceEnd, FColor::Red, false, 1.5f, 0, 2.f);

}
