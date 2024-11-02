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

	// 위치 회전 동기화 / 위치 보간
	FVector Location = GetActorLocation();
	FVector DestLocation = FVector(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z);

	FVector MoveDir = (DestLocation - Location);
	const float DistToDest = MoveDir.Length();
	MoveDir.Normalize();

	// 목표 위치까지의 평균 속도 계산 (1초 동안 일정하게 이동)
	FVector AverageVelocity = MoveDir / 1.0f;  // 1초 동안 이동할 위치를 기준으로 평균 속도 설정
	float GroundSpeed = AverageVelocity.Size(); // 속력 계산

	float MoveDist = (MoveDir * 600.f * DeltaTime).Length();
	MoveDist = FMath::Min(MoveDist, DistToDest);
	FVector NextLocation = Location + MoveDir * MoveDist;

	SetActorLocation(NextLocation);

	FRotator Rotation = GetActorRotation();
	Rotation.Yaw = PlayerInfo.Yaw;

	SetActorRotation(Rotation);

}

void AGPCharacterBase::SetPlayerInfo(FPlayerInfo& PlayerInfo_)
{
	PrevPlayerInfo = PlayerInfo;
	PlayerInfo = PlayerInfo_;

	SetAnimVar();
	
	UE_LOG(LogTemp, Warning, TEXT("Set PlayerInfo[%d] (%f,%f,%f)(%f) (%d)"),
		PlayerInfo.ID, PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z, PlayerInfo.Yaw , PlayerInfo.State);
}

void AGPCharacterBase::SetAnimVar()
{
	FVector CurLocation = FVector(PrevPlayerInfo.X , PrevPlayerInfo.Y , PrevPlayerInfo.Z);

	UE_LOG(LogTemp, Warning, TEXT(" CurLocation (%f %f %f)"),
		CurLocation.X, CurLocation.Y, CurLocation.Z);

	FVector DisLocation = FVector(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z);

	UE_LOG(LogTemp, Warning, TEXT(" DisLocation (%f %f %f)"),
		DisLocation.X, DisLocation.Y, DisLocation.Z);

	FVector Velocity = (DisLocation - CurLocation);

	// 애니메이션 동기화
	UGPPlayerAnimInstance* AnimInstance = Cast<UGPPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		if (PlayerInfo.State == STATE_IDLE)
		{
			AnimInstance->bIsIdle = true;
			AnimInstance->GroundSpeed = 0;
		}
		else
		{
			AnimInstance->bIsIdle = false;
			AnimInstance->Velocity = Velocity;
			AnimInstance->GroundSpeed = Velocity.Size2D();
		}

		UE_LOG(LogTemp, Warning, TEXT("Set Other Player Animation (%f %f %f) (%f)"),
			Velocity.X, Velocity.Y, Velocity.Z, Velocity.Size2D());
	}
}


