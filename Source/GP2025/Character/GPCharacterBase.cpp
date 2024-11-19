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

	{
		PlayerInfo.AddState(STATE_IDLE);
	}
}

void AGPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 플레이어가 로컬 플레이어이면 return
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (GameInstance && GameInstance->MyPlayer == this)
		return;

	// 위치 회전 동기화 / 위치 보간
	// 현재의 틱 위치와 서버에서 최근에 전송받은 위치를 FVector 에 저장
	FVector Location = GetActorLocation();
	FVector DestLocation = FVector(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z);

	// 이동해야 할 거리값을 DistToDest 에 저장
	FVector MoveDir = (DestLocation - Location);
	const float DistToDest = MoveDir.Length();
	MoveDir.Normalize();

	float Speed = PlayerInfo.Speed;

	float MoveDist = (MoveDir * Speed * DeltaTime).Length();
	MoveDist = FMath::Min(MoveDist, DistToDest);
	FVector NextLocation = Location + MoveDir * MoveDist;

	SetActorLocation(NextLocation);
	GetCharacterMovement()->Velocity = MoveDir * Speed;

	FRotator Rotation = GetActorRotation();
	Rotation.Yaw = PlayerInfo.Yaw;
	SetActorRotation(Rotation);

	// 점프 상태 처리: MovementComponent 활용
	if (PlayerInfo.HasState(STATE_JUMP))
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	else if (!PlayerInfo.HasState(STATE_JUMP) && NextLocation.Z < 120.f)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	if(!PlayerInfo.HasState(STATE_IDLE))
		UE_LOG(LogTemp, Warning, TEXT("DistToDest : %f, MoveDist : %f, Speed : %f"), DistToDest , MoveDist , Speed);
}

void AGPCharacterBase::SetClientInfoFromServer(FPlayerInfo& PlayerInfo_)
{
	PlayerInfo = PlayerInfo_;
}


