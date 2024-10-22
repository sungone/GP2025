// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GPCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/GPPlayerAnimInstance.h"
#include "Network/GPGameInstance.h"

// Sets default values
AGPCharacterBase::AGPCharacterBase()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/DokkaebiAssets/boss_body_bip.boss_body_bip'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animation/PlayerAnimation/ABP_GPCharacterPlayer.ABP_GPCharacterPlayer_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 800.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
}

void AGPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGPCharacterBase::SetPlayerInfo(FPlayerInfo& PlayerInfo_)
{
	PlayerInfo = PlayerInfo_;

	SetPlayerLocationAndRotation(PlayerInfo);

	UE_LOG(LogTemp, Warning, TEXT("Set PlayerInfo[%d] (%f,%f,%f)(%f)"),
		PlayerInfo.ID, PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z, PlayerInfo.Yaw);
}

void AGPCharacterBase::SetPlayerLocationAndRotation(FPlayerInfo& PlayerInfo_)
{
	PlayerInfo = PlayerInfo_;

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z);

	SetActorLocation(NewLocation);
	SetActorRotation(FRotator(0, PlayerInfo.Yaw, 0));

	if (PlayerInfo.State == STATE_IDLE)
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	// Velocity °è»ê
	FVector Velocity = (NewLocation - CurrentLocation) / DeltaTime;
	GetCharacterMovement()->Velocity = Velocity;

	UE_LOG(LogTemp, Warning, TEXT("Set PlayerInfo[%d] (%f,%f,%f)(%f)"),
		PlayerInfo.ID, PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z, PlayerInfo.Yaw);
}

