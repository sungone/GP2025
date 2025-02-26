

#include "Character/GPCharacterBase.h"
#include "Character/GPCharacterControlData.h"
#include "Character/GPCharacterMyplayer.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GPCharacterPlayer.h"
#include "Item/GPEquipItemData.h"
#include "Network/GPNetworkManager.h"
#include "Physics/GPCollision.h"
#include "UI/GPHpBarWidget.h"
#include "UI/GPLevelWidget.h"
#include "UI/GPWidgetComponent.h"
#include "TLoad.h"
#include <random>
static std::random_device rd;
static std::mt19937 gen(rd());

DEFINE_LOG_CATEGORY(LogGPCharacter);

AGPCharacterBase::AGPCharacterBase()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_GPCAPSULE);

	auto* MovementComp = GetCharacterMovement();
	MovementComp->bOrientRotationToMovement = true;
	MovementComp->RotationRate = FRotator(0.f, 500.f, 0.f);
	MovementComp->JumpZVelocity = 300.f;
	MovementComp->AirControl = 0.35f;
	MovementComp->GravityScale = 1.f;
	MovementComp->MaxWalkSpeed = 300.f;
	MovementComp->MinAnalogWalkSpeed = 20.f;
	MovementComp->BrakingDecelerationWalking = 2000.f;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	static const TArray<TTuple<ECharacterType, FString>> CharacterTypes = {
		MakeTuple(Type::EPlayer::WARRIOR, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Warrior.GPC_Warrior'")),
		MakeTuple(Type::EPlayer::GUNNER, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Gunner.GPC_Gunner'")),
		MakeTuple(Type::EMonster::BUBBLE_TEA, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_BubbleTea.GPC_BubbleTea'")),
		MakeTuple(Type::EMonster::ENERGY_DRINK, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_EnergyDrink.GPC_EnergyDrink'")),
		MakeTuple(Type::EMonster::COFFEE, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Coffee.GPC_Coffee'")),
		MakeTuple(Type::EMonster::MOUSE, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Mouse.GPC_Mouse'")),
		MakeTuple(Type::EMonster::KEYBOARD, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Keyboard.GPC_Keyboard'")),
		MakeTuple(Type::EMonster::DESKTOP, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Desktop.GPC_Desktop'")),
		MakeTuple(Type::EMonster::COGWHEEL, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Cogwheel.GPC_Cogwheel'")),
		MakeTuple(Type::EMonster::BOLT_NUT, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_BoldNut.GPC_BoldNut'")),
		MakeTuple(Type::EMonster::DRILL, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Drill.GPC_Drill'")),
		MakeTuple(Type::EMonster::TINO, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Tino.GPC_Tino'")),
	};

	LoadCharacterData(CharacterTypeManager, CharacterTypes);

	HpBar = CreateWidgetComponent(TEXT("HpWidget"), TEXT("/Game/UI/WBP_CharacterHpBar.WBP_CharacterHpBar_C"), FVector(0.f, 0.f, 300.f), FVector2D(150.f, 15.f));
	LevelText = CreateWidgetComponent(TEXT("LevelWidget"), TEXT("/Game/UI/WBP_LevelText.WBP_LevelText_C"), FVector(0.f, 0.f, 340.f), FVector2D(40.f, 10.f));
}

void AGPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 내 플레이어의 위치를 설정하는 것이면 return
	if (Cast<AGPCharacterMyplayer>(this)) return;

	/// Other Client 공격 모션 동기화 ///
	if (CharacterInfo.HasState(STATE_AUTOATTACK) && bIsAutoAttacking == false)
	{
		ProcessAutoAttackCommand();
		return;
	}

	/// 미끄러지는 문제 해결하기 ///
	{
		if (CharacterInfo.Speed < 200.f)
		{
			CharacterInfo.Speed = 300.f;
		}
	}

	/// Other Client 위치 및 회전 동기화 ///
	FVector Location = GetActorLocation();
	FVector DestLocation = CharacterInfo.Pos;
	float Speed = CharacterInfo.Speed;

	FVector MoveDir = (DestLocation - Location);
	const float DistToDest = MoveDir.Length();
	MoveDir.Normalize();

	float MoveDist = (MoveDir * Speed * DeltaTime).Length();
	MoveDist = FMath::Min(MoveDist, DistToDest);
	FVector NextLocation = Location + MoveDir * MoveDist;

	FRotator Rotation = GetActorRotation();
	Rotation.Yaw = CharacterInfo.Yaw;

	SetActorLocationAndRotation(NextLocation, Rotation);

	/// Ohter Client 속도 동기화 ///
	GetCharacterMovement()->Velocity = MoveDir * Speed;

	/// Other Client 점프 동기화 ///
	if (CharacterInfo.HasState(STATE_JUMP))
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	else if (!CharacterInfo.HasState(STATE_JUMP) && GetActorLocation().Z < 150.f)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AGPCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AGPCharacterBase::SetCharacterInfo(FInfoData& CharacterInfo_)
{
	CharacterInfo = CharacterInfo_;

	OnHpChanged.Broadcast(CharacterInfo.Stats.Hp / CharacterInfo.Stats.MaxHp);
	OnExpChanged.Broadcast(CharacterInfo.Stats.Exp / CharacterInfo.Stats.MaxExp);
	OnLevelChanged.Broadcast(CharacterInfo.Stats.Level);
}

USkeletalMeshComponent* AGPCharacterBase::GetCharacterMesh() const
{
	return GetMesh();
}

void AGPCharacterBase::ProcessAutoAttackCommand()
{
	UAnimInstance* AnimInstance = GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || !AttackActionMontage)
		return;

	if (AnimInstance->Montage_IsPlaying(AttackActionMontage))
		return;

	bIsAutoAttacking = true;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &AGPCharacterBase::OnAutoAttackMontageEnded);
	AnimInstance->Montage_Play(AttackActionMontage, 1.f);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackActionMontage);
}

void AGPCharacterBase::OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackActionMontage)
	{
		bIsAutoAttacking = false;

		if (CharacterInfo.HasState(STATE_AUTOATTACK))
		{
			CharacterInfo.RemoveState(STATE_AUTOATTACK);
		}
	}
}

void AGPCharacterBase::SetCharacterData(const UGPCharacterControlData* CharacterData)
{
	bUseControllerRotationYaw = CharacterData->bUseControllerRotationYaw;

	GetCharacterMovement()->bOrientRotationToMovement = CharacterData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterData->RotationRate;

	AttackActionMontage = CharacterData->AttackAnimMontage;
	DeadMontage = CharacterData->DeadAnimMontage;
}

void AGPCharacterBase::SetCharacterType(ECharacterType NewCharacterType)
{
	UGPCharacterControlData* NewCharacterData = CharacterTypeManager[NewCharacterType];
	check(NewCharacterData);

	SetCharacterData(NewCharacterData);

	CurrentCharacterType = NewCharacterType;
}
 
void AGPCharacterBase::AttackHitCheck()
{
	//Todo: Myplayer만 사용하게 옮기자
	if (!Cast<AGPCharacterMyplayer>(this)) return;

	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = CharacterInfo.AttackRange;
	const float AttackRadius = CharacterInfo.AttackRadius;

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool bHitDetected = GetWorld()->SweepSingleByChannel(
		OutHitResult, Start, End, FQuat::Identity, CCHANNEL_GPACTION,
		FCollisionShape::MakeSphere(AttackRadius), Params);

	if (bHitDetected && IsValid(OutHitResult.GetActor()))
	{
		AGPCharacterBase* TargetCharacter = Cast<AGPCharacterBase>(OutHitResult.GetActor());
		if (IsValid(TargetCharacter))
		{
			const FInfoData& TargetInfo = TargetCharacter->CharacterInfo;

			const FVector TargetLocation = TargetCharacter->GetActorLocation();
			const float TargetCollisionRadius = TargetInfo.CollisionRadius;
			const float TargetHalfHeight = TargetCollisionRadius;

#if ENABLE_DRAW_DEBUG
			DrawDebugCapsule(GetWorld(), TargetLocation, TargetHalfHeight, TargetCollisionRadius,
				FQuat::Identity, FColor::Yellow, false, 5.f);
#endif
			auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
			if (NetworkMgr)
			{
				NetworkMgr->SendPlayerAttackPacket(TargetCharacter->CharacterInfo.ID);
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	const FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	const float CapsuleHalfHeight = AttackRange * 0.5f;
	const FColor DrawColor = FColor::Red;
	if (bHitDetected)
	{
		DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius,
			FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
			DrawColor, false, 5.f);
	}
#endif
}



UGPWidgetComponent* AGPCharacterBase::CreateWidgetComponent(const FString& Name, const FString& WidgetPath, FVector Location, FVector2D Size)
{
	UGPWidgetComponent* WidgetComp = CreateDefaultSubobject<UGPWidgetComponent>(*Name);
	WidgetComp->SetupAttachment(GetCharacterMesh());
	WidgetComp->SetComponent(Location, Size);
	WidgetComp->SetWidgetClass(LoadClass<UUserWidget>(WidgetPath));

	return WidgetComp;
}

void AGPCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda(
		[&]()
		{
			Destroy();
		}
	), DeadEventDelayTime, false); 
}

void AGPCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	AnimInstance->StopAllMontages(0.f);
	AnimInstance->Montage_Play(DeadMontage, 1.f);
}
