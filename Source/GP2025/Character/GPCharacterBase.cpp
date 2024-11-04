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
	{
		return;
	}

	UGPPlayerAnimInstance* AnimInstance = Cast<UGPPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (!AnimInstance)
		return;

	// 위치 회전 동기화 / 위치 보간
	FVector Location = GetActorLocation();
	FVector DestLocation = FVector(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z);

	float Speed = PlayerInfo.Speed;

	UE_LOG(LogTemp, Warning, TEXT("Ground Speed :  (%f)"),
		Speed);

	FVector MoveDir = (DestLocation - Location);
	const float DistToDest = MoveDir.Length();
	MoveDir.Normalize();

	float MoveDist = (MoveDir * Speed * DeltaTime).Length();
	MoveDist = FMath::Min(MoveDist, DistToDest);
	FVector NextLocation = Location + MoveDir * MoveDist;

	SetActorLocation(NextLocation);

	GetCharacterMovement()->Velocity = MoveDir * Speed;

	FRotator Rotation = GetActorRotation();
	Rotation.Yaw = PlayerInfo.Yaw;

	SetActorRotation(Rotation);
}

void AGPCharacterBase::SetPlayerInfo(FPlayerInfo& PlayerInfo_)
{
	PlayerInfo = PlayerInfo_;
	SetPlayerInfoMessage();
}

void AGPCharacterBase::SetPlayerInfoMessage()
{
	FString StateString;

	// 각 상태에 대해 설정 여부를 검사하고 문자열에 추가
	if (PlayerInfo.State & STATE_IDLE) StateString += TEXT("IDLE ");
	if (PlayerInfo.State & STATE_WALK) StateString += TEXT("WALK ");
	if (PlayerInfo.State & STATE_RUN) StateString += TEXT("RUN ");
	if (PlayerInfo.State & STATE_JUMP) StateString += TEXT("JUMP ");

	// 비트가 모두 설정되지 않았을 경우
	if (StateString.IsEmpty())
	{
		StateString = TEXT("NONE");
	}

	UE_LOG(LogTemp, Warning, TEXT("Set PlayerInfo[%d] (%f, %f, %f)(%f) (%s)"),
		PlayerInfo.ID, PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z, PlayerInfo.Yaw, *StateString);
}

