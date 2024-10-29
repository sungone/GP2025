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

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 플레이어가 로컬 플레이어 인지 확인
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (GameInstance && GameInstance->MyPlayer == this)
	{
		return;
	}

	FVector CurrentLocation = GetActorLocation();
	FRotator CurrentRotation = GetActorRotation();

	if (DeltaTime > 0)
	{
		// 이전 위치와 현재 위치의 차이를 이용해 Velocity 계산
		FVector CalculatedVelocity = (CurrentLocation - PreviousLocation) / DeltaTime;
		float Speed = CalculatedVelocity.Size2D();

		// 애니메이션 인스턴스에 Velocity와 애니메이션 상태 업데이트
		UGPPlayerAnimInstance* AnimInstance = Cast<UGPPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (AnimInstance)
		{
			AnimInstance->Velocity = CalculatedVelocity;
			AnimInstance->GroundSpeed = Speed;
			AnimInstance->bIsIdle = Speed < AnimInstance->MovingThreshould;

			// 원격 캐릭터를 부드럽게 목표 위치로 이동
			float InterpSpeed = FMath::Max(Speed, 300.f); // 최소 속도 설정 (300.0f 이상인 경우만 반영)
			FVector NewCalculatedLocation = FMath::VInterpConstantTo(CurrentLocation, NewLocation, DeltaTime, InterpSpeed);
			FRotator NewCalculatedRotation = FMath::RInterpTo(CurrentRotation, NewRotation, DeltaTime, 5.0f);

			SetActorLocation(NewCalculatedLocation);
			SetActorRotation(NewCalculatedRotation);

			
		}

		PreviousLocation = CurrentLocation;
	}
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

	NewLocation = FVector(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z);
	NewRotation = FRotator(0, PlayerInfo.Yaw, 0);
}
