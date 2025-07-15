

#include "Character/GPCharacterBase.h"
#include "Character/GPCharacterControlData.h"
#include "Character/GPCharacterMyplayer.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GPCharacterPlayer.h"
#include "Network/GPNetworkManager.h"
#include "Physics/GPCollision.h"
#include "Animation/GPCharacterAnimInstance.h"
#include "UI/GPHpBarWidget.h"
#include "UI/GPLevelWidget.h"
#include "UI/GPWidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h" 
#include "UI/GPUserNameWidget.h"
#include "Character/Modules/GPCharacterCombatHandler.h"
#include "Character/Modules/GPCharacterUIHandler.h"
#include "Kismet/GameplayStatics.h"

#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());

DEFINE_LOG_CATEGORY(LogGPCharacter);
namespace
{
	template <typename T>
	T* LoadAsset(const FString& Path)
	{
		ConstructorHelpers::FObjectFinder<T> AssetRef(*Path);
		return AssetRef.Object;
	}

	template <typename T>
	TSubclassOf<T> LoadClass(const FString& Path)
	{
		ConstructorHelpers::FClassFinder<T> ClassRef(*Path);
		return ClassRef.Class;
	}

	template <typename EnumType>
	void LoadCharacterData(TMap<EnumType, UGPCharacterControlData*>& Manager, const TArray<TTuple<EnumType, FString>>& DataArray)
	{
		for (const auto& Data : DataArray)
		{
			ConstructorHelpers::FObjectFinder<UGPCharacterControlData> DataRef(*Data.Value);
			if (DataRef.Object)
			{
				Manager.Add(Data.Key, DataRef.Object);
			}
		}
	}
}

AGPCharacterBase::AGPCharacterBase()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	// 캡슐 초기화
	auto* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		const float CapsuleRadius = 42.f;
		const float CapsuleHalfHeight = 99.f;

		Capsule->InitCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
		Capsule->SetHiddenInGame(false);
		Capsule->SetVisibility(true);
	}

	// 이동 설정
	auto* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->RotationRate = FRotator(0.f, 500.f, 0.f);
	}

	// 메시 위치 정렬
	auto* MeshComp = GetMesh();
	if (MeshComp && Capsule)
	{
		const float MeshOffsetZ = -Capsule->GetUnscaledCapsuleHalfHeight();
		MeshComp->SetWorldScale3D(FVector(0.65f));
		MeshComp->SetRelativeLocationAndRotation(FVector(0.f, 0.f, MeshOffsetZ), FRotator(0.f, -90.f, 0.f));
		MeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
	}


	static const TArray<TTuple<ECharacterType, FString>> CharacterTypes = {
		MakeTuple((uint8)Type::EPlayer::WARRIOR, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Warrior.GPC_Warrior'")),
		MakeTuple((uint8)Type::EPlayer::GUNNER, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Gunner.GPC_Gunner'")),
		MakeTuple((uint8)Type::EMonster::BUBBLE_TEA, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_BubbleTea.GPC_BubbleTea'")),
		MakeTuple((uint8)Type::EMonster::ENERGY_DRINK, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_EnergyDrink.GPC_EnergyDrink'")),
		MakeTuple((uint8)Type::EMonster::COFFEE, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Coffee.GPC_Coffee'")),
		MakeTuple((uint8)Type::EMonster::MOUSE, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Mouse.GPC_Mouse'")),
		MakeTuple((uint8)Type::EMonster::KEYBOARD, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Keyboard.GPC_Keyboard'")),
		MakeTuple((uint8)Type::EMonster::DESKMON, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Deskmon.GPC_Deskmon'")),
		MakeTuple((uint8)Type::EMonster::COGWHEEL, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Cogwheel.GPC_Cogwheel'")),
		MakeTuple((uint8)Type::EMonster::BOLT_NUT, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_BoldNut.GPC_BoldNut'")),
		MakeTuple((uint8)Type::EMonster::DRILL, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Drill.GPC_Drill'")),
		MakeTuple((uint8)Type::EMonster::TINO, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Tino.GPC_Tino'")),
	};

	LoadCharacterData(CharacterTypeManager, CharacterTypes);
}

void AGPCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (!CombatHandler)
	{
		CombatHandler = NewObject<UGPCharacterCombatHandler>(this, UGPCharacterCombatHandler::StaticClass());
		if (CombatHandler)
		{
			CombatHandler->Initialize(this);
		}
	}
	
	if (!UIHandler)
	{
		UIHandler = NewObject<UGPCharacterUIHandler>(this);
		if (UIHandler)
		{
			UIHandler->Initialize(this);          
		}
	}

#if WITH_EDITOR
	GetCapsuleComponent()->SetHiddenInGame(false);
#else
	GetCapsuleComponent()->SetHiddenInGame(true);
#endif
}

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugCollisionAndAttackRadius(GetWorld(), CharacterInfo);

	if (Cast<AGPCharacterMyplayer>(this)) return;

	// Attack Sync
	HandleQSkillState();
	HandleESkillState();
	HandleRSkillState();
	HandleAutoAttackState();

	// Move Sync
	HandleRemoteMovementSync(DeltaTime);

	// Jump Sync
	HandleRemoteJumpSync();
}

void AGPCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


void AGPCharacterBase::SetCharacterInfo(const FInfoData& CharacterInfo_)
{
	CharacterInfo = CharacterInfo_;

	OnHpChanged.Broadcast(CharacterInfo.Stats.Hp / CharacterInfo.Stats.MaxHp);
	OnLevelChanged.Broadcast(CharacterInfo.Stats.Level);
}

void AGPCharacterBase::SetNameByCharacterInfo()
{
	UIHandler->SetNameByCharacterInfo();
}

void AGPCharacterBase::HandleAutoAttackState()
{
	if (CharacterInfo.HasState(STATE_AUTOATTACK) && !CombatHandler->IsAutoAttacking() 
		&& !CombatHandler->IsUsingSkill())
	{
		CombatHandler->PlayAutoAttackMontage();
	}
}

void AGPCharacterBase::HandleQSkillState()
{
	if (CharacterInfo.HasState(STATE_SKILL_Q) && !CombatHandler->IsUsingSkill() 
		&& !CombatHandler->IsAutoAttacking())
	{
		CombatHandler->PlayQSkillMontage();
	}
}

void AGPCharacterBase::HandleESkillState()
{
	if (CharacterInfo.HasState(STATE_SKILL_E) && !CombatHandler->IsUsingSkill() 
		&& !CombatHandler->IsAutoAttacking())
	{
		CombatHandler->PlayESkillMontage();
	}
}

void AGPCharacterBase::HandleRSkillState()
{
	if (CharacterInfo.HasState(STATE_SKILL_R) && !CombatHandler->IsUsingSkill() 
		&& !CombatHandler->IsAutoAttacking())
	{
		CombatHandler->PlayRSkillMontage();
	}
}

void AGPCharacterBase::HandleRemoteMovementSync(float DeltaTime)
{
	if (DeltaTime <= 0.f)
		return;

	FVector CurrentLocation = GetActorLocation();
	FVector TargetLocation = CharacterInfo.Pos;

	const float SprintSpeed = 1200.f;
	const float WalkSpeed = 600.f;
	float Speed = CharacterInfo.HasState(STATE_RUN) ? SprintSpeed : WalkSpeed;

	FVector MoveDir = TargetLocation - CurrentLocation;
	float DistanceToTarget = MoveDir.Size();

	if (DistanceToTarget < KINDA_SMALL_NUMBER)
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		return;
	}

	MoveDir.Normalize();

	float MoveDistance = FMath::Min(Speed * DeltaTime, DistanceToTarget);
	FVector NextLocation = CurrentLocation + MoveDir * MoveDistance;

	FRotator TargetRotation(0.f, CharacterInfo.Yaw, 0.f);
	FRotator InterpolatedRotation = FMath::RInterpTo(
		GetActorRotation(),
		TargetRotation,
		DeltaTime,
		10.f
	);

	SetActorLocationAndRotation(NextLocation, InterpolatedRotation, /*bSweep=*/ false);

	FVector NewLocation = GetActorLocation();
	FVector DeltaMove = (NewLocation - CurrentLocation);

	if (DeltaMove.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		FVector NewVelocity = DeltaMove / DeltaTime;
		GetCharacterMovement()->Velocity = NewVelocity;
	}
	else
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

void AGPCharacterBase::HandleRemoteJumpSync()
{
	// Z값 동기화
	GroundTraceElapsedTime += GetWorld()->GetDeltaSeconds();
	if (GroundTraceElapsedTime >= GroundTraceInterval)
	{
		UpdateGroundZLocation();
		GroundTraceElapsedTime = 0.f;
	}

	// 실제 점프 동기화
	const float ZAcceptableRangeValues = 15.f;
	float DeltaZ = FMath::Abs(GetActorLocation().Z - Ground_ZLocation);

	if (CharacterInfo.HasState(STATE_JUMP))
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	else if (DeltaZ < ZAcceptableRangeValues)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AGPCharacterBase::SetGroundZLocation(float Z)
{
	Ground_ZLocation = Z;
}

void AGPCharacterBase::UpdateGroundZLocation()
{
	FHitResult Hit;
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 1000.0f);

	FCollisionQueryParams Params(NAME_None, false, this);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		const float CapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
		Ground_ZLocation = Hit.ImpactPoint.Z + CapsuleHalfHeight;
	}
}

USkeletalMeshComponent* AGPCharacterBase::GetCharacterMesh() const
{
	return GetMesh();
}

void AGPCharacterBase::SetCharacterData(const UGPCharacterControlData* CharacterData)
{
	bUseControllerRotationYaw = CharacterData->bUseControllerRotationYaw;

	GetCharacterMovement()->bOrientRotationToMovement = CharacterData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterData->RotationRate;

	if (CombatHandler)
	{
		CombatHandler->SetAttackWithoutWeaponMontage(CharacterData->AttackWithoutWeaponAnimMontage);
		CombatHandler->SetAttackMontage(CharacterData->AttackAnimMontage);
		CombatHandler->SetDeadMontage(CharacterData->DeadAnimMontage);
		CombatHandler->SetQSkillMontage(CharacterData->QSkillAnimMontage);  
		CombatHandler->SetESkillMontage(CharacterData->ESkillAnimMontage);  
		CombatHandler->SetRSkillMontage(CharacterData->RSkillAnimMontage); 

		if (CharacterData->AttackAnimMontage_2 && CharacterData->AttackAnimMontage_3)
		{
			CombatHandler->SetAttackMontageForWarrior(CharacterData->AttackAnimMontage , 
				CharacterData->AttackAnimMontage_2 , CharacterData->AttackAnimMontage_3);
		}
	}
}

void AGPCharacterBase::SetCharacterType(ECharacterType NewCharacterType)
{
	UGPCharacterControlData* NewCharacterData = CharacterTypeManager[NewCharacterType];
	check(NewCharacterData);

	CurrentCharacterType = NewCharacterType;
	SetCharacterData(NewCharacterData);
}