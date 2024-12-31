

#include "Character/GPCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/GPPlayerAnimInstance.h"
#include "Network/GPGameInstance.h"
#include "Character/GPCharacterControlData.h"
#include "Animation/AnimMontage.h"

// Sets default values
AGPCharacterBase::AGPCharacterBase()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/temporaryAssets/ManPlayer/Man.Man'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animation/WarriorAnimation/ABP_Warrior.ABP_Warrior_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AutoAttackMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/WarriorAnimation/AM_WarriorAttack.AM_WarriorAttack'"));
	if (AutoAttackMontageRef.Object)
	{
		AutoAttackActionMontage = AutoAttackMontageRef.Object;
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
		PlayerInfo.State = 1; // IDLE
	}

	// Character Control 설정
	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> WarriorDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterControl/GPC_Warrior.GPC_Warrior'"));
	if (WarriorDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Warrior, WarriorDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> GunnerDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterControl/GPC_Gunner.GPC_Gunner'"));
	if (GunnerDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Gunner, GunnerDataRef.Object);
	}

}

void AGPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 내 플레이어의 위치를 설정하는 것이면 return
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (GameInstance && GameInstance->MyPlayer == this)
		return;

	/// Other Client 공격 모션 동기화 ///
	if (PlayerInfo.HasState(STATE_AUTOATTACK) && bIsAutoAttacking == false)
	{
		ProcessAutoAttackCommand();
		return;
	}

	/// 미끄러지는 문제 해결하기 ///
	{
		if (PlayerInfo.Speed < 200.f)
		{
			PlayerInfo.Speed = 300.f;
		}
	}

	/// Other Client 위치 및 회전 동기화 ///
	FVector Location = GetActorLocation();
	FVector DestLocation = FVector(PlayerInfo.X, PlayerInfo.Y, PlayerInfo.Z);
	float Speed = PlayerInfo.Speed;

	FVector MoveDir = (DestLocation - Location);
	const float DistToDest = MoveDir.Length();
	MoveDir.Normalize();

	float MoveDist = (MoveDir * Speed * DeltaTime).Length();
	MoveDist = FMath::Min(MoveDist, DistToDest);
	FVector NextLocation = Location + MoveDir * MoveDist;

	FRotator Rotation = GetActorRotation();
	Rotation.Yaw = PlayerInfo.Yaw;

	SetActorLocationAndRotation(NextLocation, Rotation);

	/// Ohter Client 속도 동기화 ///
	GetCharacterMovement()->Velocity = MoveDir * Speed;

	/// Other Client 점프 동기화 ///
	if (PlayerInfo.HasState(STATE_JUMP))
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	else if (!PlayerInfo.HasState(STATE_JUMP) && GetActorLocation().Z < 120.f)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AGPCharacterBase::SetClientInfoFromServer(FPlayerInfo& PlayerInfo_)
{
	PlayerInfo = PlayerInfo_;
}

void AGPCharacterBase::ProcessAutoAttackCommand()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !AutoAttackActionMontage)
		return;

	if (AnimInstance->Montage_IsPlaying(AutoAttackActionMontage))
		return;

	bIsAutoAttacking = true;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &AGPCharacterBase::OnAutoAttackMontageEnded);
	AnimInstance->Montage_Play(AutoAttackActionMontage , 1.f);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AutoAttackActionMontage);
}

void AGPCharacterBase::OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AutoAttackActionMontage)
	{
		bIsAutoAttacking = false;

		if (PlayerInfo.HasState(STATE_AUTOATTACK))
		{
			PlayerInfo.RemoveState(STATE_AUTOATTACK);
		}
	}
}

void AGPCharacterBase::SetCharacterControlData(const UGPCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}


