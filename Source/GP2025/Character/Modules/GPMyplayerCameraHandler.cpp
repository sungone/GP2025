// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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

	// NPC 상호작용 시 카메라 처리
	if (!(!PlayerSpringArm || !PlayerController || !PlayerPawn))
	{
		// 1. 카메라 회전
		if (bIsLookingAtTarget)
		{
			FRotator CurrentRot = PlayerController->GetControlRotation();
			FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(PlayerPawn->GetActorLocation(), LookAtTarget);
			TargetRot.Yaw -= 270.f;
			TargetRot.Pitch = -5.f;
			TargetRot.Roll = 0.f;

			FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 5.f);
			PlayerController->SetControlRotation(NewRot);
		}

		// 2. 카메라 줌 조절
		if (bIsZoomingForDialogue)
		{
			float CurrentLength = PlayerSpringArm->TargetArmLength;
			float NewLength = FMath::FInterpTo(CurrentLength, DialogueZoomTargetLength, DeltaTime, 5.f);
			PlayerSpringArm->TargetArmLength = NewLength;
		}
		else
		{
			// 원래 거리로 복귀
			float CurrentLength = PlayerSpringArm->TargetArmLength;
			float NewLength = FMath::FInterpTo(CurrentLength, DefaultZoomLength, DeltaTime, 5.f);
			PlayerSpringArm->TargetArmLength = NewLength;
		}
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

void UGPMyplayerCameraHandler::StartDialogueCamera(const FVector& TargetLocation)
{
	if (!Owner) return;

	PlayerController = Cast<APlayerController>(Owner->GetController());
	PlayerPawn = Cast<APawn>(Owner);

	PlayerSpringArm = Owner->FindComponentByClass<USpringArmComponent>();
	if (!PlayerSpringArm) return;

	LookAtTarget = TargetLocation;
	bIsLookingAtTarget = true;
	bIsZoomingForDialogue = true;
}

void UGPMyplayerCameraHandler::StopDialogueCamera()
{
	bIsLookingAtTarget = false;
	bIsZoomingForDialogue = false;
}
