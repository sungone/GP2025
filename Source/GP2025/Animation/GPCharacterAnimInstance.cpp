// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/GPCharacterAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/GPCharacterMyplayer.h" 

UGPCharacterAnimInstance::UGPCharacterAnimInstance()
{
	MovingThreshould = 10.f;
	JumpingThreshould = 100.f;
}

void UGPCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Owner = Cast<ACharacter>(GetOwningActor());

	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UGPCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!IsValid(Owner))
    {
        Owner = Cast<ACharacter>(GetOwningActor());
        if (Owner)
        {
            Movement = Owner->GetCharacterMovement();
        }
    }

    if (Owner && Movement)
    {
        Velocity = Movement->Velocity;
        GroundSpeed = Velocity.Size2D();
        bIsIdle = GroundSpeed < MovingThreshould;
        bIsFalling = Movement->IsFalling();
        bIsJumping = bIsFalling && (Velocity.Z > JumpingThreshould);

        // Gunner Player Zooming ÆÇ´Ü
        AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(Owner);
        if (MyPlayer && MyPlayer->CameraHandler)
        {
            bIsZooming = MyPlayer->CameraHandler->IsZooming();
        }
        else
        {
            AGPCharacterPlayer* Player = Cast<AGPCharacterPlayer>(Owner);
            if (Player)
            {
                bIsZooming = Player->CharacterInfo.HasState(STATE_AIMING);
            }
        }
    }
    else
    {
        Velocity = FVector::ZeroVector;
        GroundSpeed = 0.f;
        bIsIdle = true;
        bIsFalling = false;
        bIsJumping = false;
        bIsZooming = false;
    }
}
