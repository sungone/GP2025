// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

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
	if (!Owner || !Owner->FollowCamera || !Owner->CameraBoom) return;

	// === 1. �� ���� ó�� ===
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

	// === 2. NPC ��ȣ�ۿ� ī�޶� ȸ��/�� ===
	if (PlayerSpringArm && PlayerController && PlayerPawn)
	{
		// ȸ��
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

		// ��
		const float CurrentLength = PlayerSpringArm->TargetArmLength;
		const float DesiredLength = bIsZoomingForDialogue ? DialogueZoomTargetLength : DefaultZoomLength;
		PlayerSpringArm->TargetArmLength = FMath::FInterpTo(CurrentLength, DesiredLength, DeltaTime, 5.f);
	}

	// === 3. ī�޶� ��鸲 ���� ===
	if (bRestoreArmLength)
	{
		// ���� �ӵ��� ����
		Owner->CameraBoom->TargetArmLength = FMath::FInterpConstantTo(
			Owner->CameraBoom->TargetArmLength,
			OriginalArmLength,
			DeltaTime,
			100.f // ���� �ӵ� (�������� ������)
		);

		// ���� ���������� ���� �� ����
		if (FMath::Abs(Owner->CameraBoom->TargetArmLength - OriginalArmLength) < 1.f)
		{
			Owner->CameraBoom->TargetArmLength = OriginalArmLength;
			bRestoreArmLength = false;
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
#if UE_BUILD_SHIPPING || PLATFORM_ANDROID
	// Shipping �Ǵ� Android �÷��������� ������ �ڵ常
	return bWantsToZoom;
#else
	// ���� �÷���(PC ��)������ ����� ������ this üũ ���
	return this && bWantsToZoom;
#endif
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

//void UGPMyplayerCameraHandler::PlayHitCameraShake()
//{
//	if (!Owner || !Owner->CameraBoom) return;
//
//	// ���� SocketOffset ����
//	OriginalSocketOffset = Owner->CameraBoom->SocketOffset;
//
//	// ��� Offset ����
//	FVector ShakeOffset = FVector(
//		FMath::RandRange(-30.f, 30.f),
//		FMath::RandRange(-30.f, 30.f),
//		FMath::RandRange(-4.f, 4.f)
//	);
//
//	// ����
//	Owner->CameraBoom->SocketOffset = OriginalSocketOffset + ShakeOffset;
//
//	// ��� �� ����
//	Owner->GetWorldTimerManager().SetTimer(
//		CameraShakeResetTimer,
//		this,
//		&UGPMyplayerCameraHandler::ResetCameraShake,
//		0.05f,
//		false
//	);
//}

void UGPMyplayerCameraHandler::PlayHitCameraShake()
{
	if (!Owner || !Owner->CameraBoom) return;

	// 1. �������� �� ����
	OriginalSocketOffset = Owner->CameraBoom->SocketOffset;
	OriginalArmLength = Owner->CameraBoom->TargetArmLength;

	// 2. �� ���� ���� ������
	FVector ShakeOffset = FVector(
		FMath::RandRange(-6.f, 6.f),
		FMath::RandRange(-3.f, 3.f),
		FMath::RandRange(-20.f, -10.f)
	);
	Owner->CameraBoom->SocketOffset = OriginalSocketOffset + ShakeOffset;
	bIsShaking = true;

	// 3. ���������� ī�޶� �Ÿ� �ø�
	Owner->CameraBoom->TargetArmLength += 50.f;
	bRestoreArmLength = true;
}

void UGPMyplayerCameraHandler::ResetCameraShake()
{
	// ��� �� �ص� ������ ���� ��� ȣȯ
	if (!Owner || !Owner->CameraBoom) return;
	Owner->CameraBoom->SocketOffset = OriginalSocketOffset;
	bIsShaking = false;
}

//
//void UGPMyplayerCameraHandler::ResetCameraShake()
//{
//	if (!Owner || !Owner->CameraBoom) return;
//
//	Owner->CameraBoom->SocketOffset = OriginalSocketOffset;
//}
