// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


void UGPMyplayerCameraHandler::Initialize(AGPCharacterMyplayer* InOwner)
{
	Owner = InOwner;

	if (Owner->FollowCamera)
	{
		DefaultFOV = Owner->FollowCamera->FieldOfView;
	}
	if (Owner->CameraBoom)
	{
		DefaultCameraOffset = Owner->CameraBoom->GetRelativeLocation();
		DefaultArmLength = Owner->CameraBoom->TargetArmLength;
	}

	Owner->CameraBoom->TargetArmLength = 400.f;
	Owner->CameraBoom->bUsePawnControlRotation = true;
	Owner->FollowCamera->bUsePawnControlRotation = false;
	Owner->CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	Owner->CameraBoom->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

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
