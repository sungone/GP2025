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

	// ���� �÷��̾ ���� �÷��̾� ���� Ȯ��
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (GameInstance && GameInstance->MyPlayer == this)
	{
		return;
	}

	// ��ġ ȸ�� ����ȭ / ��ġ ����
	FVector Location = GetActorLocation();
	FVector DestLocation = FVector(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z);

	FVector MoveDir = (DestLocation - Location);
	const float DistToDest = MoveDir.Length();
	MoveDir.Normalize();

	// ��ǥ ��ġ������ ��� �ӵ� ��� (1�� ���� �����ϰ� �̵�)
	FVector AverageVelocity = MoveDir / 1.0f;  // 1�� ���� �̵��� ��ġ�� �������� ��� �ӵ� ����
	float GroundSpeed = AverageVelocity.Size(); // �ӷ� ���

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

	// �ִϸ��̼� ����ȭ
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


