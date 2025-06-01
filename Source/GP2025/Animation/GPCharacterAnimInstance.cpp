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
	if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(Owner))
	{
		CachedCameraHandler = MyPlayer->CameraHandler; 
	}

}

void UGPCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Owner)
	{
		Owner = Cast<AGPCharacterBase>(TryGetPawnOwner());
		if (Owner)
		{
			Movement = Owner->GetCharacterMovement();

			if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(Owner))
			{
				CachedCameraHandler = MyPlayer->CameraHandler;
			}
		}
	}

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < MovingThreshould;
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshould);
	}

	if (Owner)
	{
		if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(Owner))
		{
			bIsZooming = (CachedCameraHandler != nullptr) ? CachedCameraHandler->IsZooming() : false;
		}
		else if (AGPCharacterPlayer* Player = Cast<AGPCharacterPlayer>(Owner))
		{
			bIsZooming = Player->CharacterInfo.HasState(STATE_AIMING);
		}
	}
}
