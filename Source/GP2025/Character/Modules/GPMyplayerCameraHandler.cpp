// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


void UGPMyplayerCameraHandler::Initialize(AGPCharacterMyplayer* InOwner)
{
	Owner = InOwner;

	Owner->CameraBoom->TargetArmLength = DefaultArmLength;
	Owner->CameraBoom->SetRelativeLocation(DefaultCameraOffset);
	Owner->CameraBoom->SetRelativeRotation(DefaultCameraRotationOffset);

	Owner->CameraBoom->bUsePawnControlRotation = true;
	Owner->CameraBoom->bUsePawnControlRotation = true;
	Owner->CameraBoom->bDoCollisionTest = true;
	Owner->CameraBoom->ProbeChannel = ECC_Camera;

	Owner->FollowCamera->bUsePawnControlRotation = false;
	Owner->FollowCamera->SetFieldOfView(DefaultFOV);
}

void UGPMyplayerCameraHandler::Tick(float DeltaTime)
{
	if (!Owner->FollowCamera || !Owner->CameraBoom) return;

	const float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	Owner->FollowCamera->SetFieldOfView(FMath::FInterpTo(Owner->FollowCamera->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed));

	const float TargetArmLength = bWantsToZoom ? ZoomedArmLength : DefaultArmLength;
	const FVector TargetOffset = bWantsToZoom ? ZoomedCameraOffset : DefaultCameraOffset;

	Owner->CameraBoom->TargetArmLength = FMath::FInterpTo(Owner->CameraBoom->TargetArmLength, TargetArmLength, DeltaTime, ZoomInterpSpeed);
	Owner->CameraBoom->SetRelativeLocation(FMath::VInterpTo(Owner->CameraBoom->GetRelativeLocation(), TargetOffset, DeltaTime, ZoomInterpSpeed));

	if (Owner)
	{
		Owner->GetCharacterMovement()->bOrientRotationToMovement = !bWantsToZoom;
		Owner->bUseControllerRotationYaw = bWantsToZoom;
	}
}

void UGPMyplayerCameraHandler::StartZoom()
{
	bWantsToZoom = true;
}

void UGPMyplayerCameraHandler::StopZoom()
{
	bWantsToZoom = false;
}

bool UGPMyplayerCameraHandler::IsZooming() const
{
	return this && bWantsToZoom;
}

void UGPMyplayerCameraHandler::ConfigureCameraCollision()
{
	if (!Owner || !Owner->CameraBoom)
		return;

	Owner->CameraBoom->bDoCollisionTest = true;
	Owner->CameraBoom->ProbeChannel = ECC_Camera;
	Owner->CameraBoom->bUsePawnControlRotation = true;
}
