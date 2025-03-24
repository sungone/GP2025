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

void AGPGun::StartAttack()
{
	// AttackHitCheck();
}

void AGPGun::AttackHitCheck()
{
    AGPCharacterPlayer* PlayerCharacter = Cast<AGPCharacterPlayer>(GetOwner());
    if (!PlayerCharacter) return;

    USkeletalMeshComponent* BodyMesh = PlayerCharacter->GetCharacterMesh(); 
    if (!BodyMesh) return;

	const FTransform SocketTransform = BodyMesh->GetSocketTransform(TEXT("WeaponSocket"));
	FVector StartLocation = SocketTransform.GetLocation();
	FVector ForwardVector = SocketTransform.GetRotation().GetAxisY();


    FVector EndLocation = StartLocation + ForwardVector * ValidRange;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    Params.AddIgnoredActor(GetOwner());

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult, StartLocation, EndLocation, CCHANNEL_GPACTION, Params);

    DrawDebugLine(
        GetWorld(),
        StartLocation,
        EndLocation,
        FColor::Red,
        false,
        2.0f,
        0,
        2.0f
    );

    if (bHit)
    {
        if (HitResult.GetActor())
        {
            UE_LOG(LogTemp, Warning, TEXT("Hitted Actor: %s"), *HitResult.GetActor()->GetName());
        }

        AGPCharacterBase* TargetCharacter = Cast<AGPCharacterBase>(HitResult.GetActor());
        if (TargetCharacter)
        {         
            auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
            if (NetworkMgr)
            {
                NetworkMgr->SendPlayerAttackPacket(TargetCharacter->CharacterInfo.ID);
                UE_LOG(LogTemp, Warning, TEXT("SendPlayerAttackPacket Completed / Gun.."));
            }
        }
    }

}
