

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
#include "UI/GPHpBarWidget.h"
#include "Weapons/GPSword.h"
#include "UI/GPLevelWidget.h"
#include "UI/GPWidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h" 

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
	auto* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->InitCapsuleSize(42.f, 96.f);
		Capsule->SetHiddenInGame(false);
		Capsule->SetVisibility(true);
	}

	auto* MovementComp = GetCharacterMovement();
	MovementComp->bOrientRotationToMovement = true;
	MovementComp->RotationRate = FRotator(0.f, 500.f, 0.f);

	GetMesh()->SetWorldScale3D(FVector(0.65f));
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	static const TArray<TTuple<ECharacterType, FString>> CharacterTypes = {
		MakeTuple((uint8)Type::EPlayer::WARRIOR, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Warrior.GPC_Warrior'")),
		MakeTuple((uint8)Type::EPlayer::GUNNER, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Gunner.GPC_Gunner'")),
		MakeTuple((uint8)Type::EMonster::BUBBLE_TEA, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_BubbleTea.GPC_BubbleTea'")),
		MakeTuple((uint8)Type::EMonster::ENERGY_DRINK, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_EnergyDrink.GPC_EnergyDrink'")),
		MakeTuple((uint8)Type::EMonster::COFFEE, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Coffee.GPC_Coffee'")),
		MakeTuple((uint8)Type::EMonster::MOUSE, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Mouse.GPC_Mouse'")),
		MakeTuple((uint8)Type::EMonster::KEYBOARD, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Keyboard.GPC_Keyboard'")),
		MakeTuple((uint8)Type::EMonster::DESKTOP, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Desktop.GPC_Desktop'")),
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
}

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugCollisionAndAttackRadius(GetWorld(), CharacterInfo);

	if (Cast<AGPCharacterMyplayer>(this)) return;

	if (CharacterInfo.HasState(STATE_AUTOATTACK) && bIsAutoAttacking == false)
	{
		ProcessAutoAttackCommand();
		return;
	}

	/// Other Client 위치 및 회전 동기화 ///
	FVector Location = GetActorLocation();
	FVector DestLocation = CharacterInfo.Pos;
	if (CharacterInfo.Stats.Speed < 400.f)
	{
		CharacterInfo.Stats.Speed = 500.f;
	}
	float Speed = CharacterInfo.Stats.Speed;

	FVector MoveDir = (DestLocation - Location);
	const float DistToDest = MoveDir.Length();
	MoveDir.Normalize();

	float MoveDist = (MoveDir * Speed * DeltaTime).Length();
	MoveDist = FMath::Min(MoveDist, DistToDest);
	FVector NextLocation = Location + MoveDir * MoveDist;

	// 회전 보간 추가
	FRotator CurrentRotation = GetActorRotation();
	FRotator TargetRotation(0.f, CharacterInfo.Yaw, 0.f);
	FRotator InterpolatedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.0f);

	SetActorLocationAndRotation(NextLocation, InterpolatedRotation);

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
	AnimInstance->Montage_Play(AttackActionMontage, 2.f);
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

UGPWidgetComponent* AGPCharacterBase::CreateWidgetComponent(const FString& Name, const FString& WidgetPath, FVector Location, FVector2D Size, UUserWidget*& OutUserWidget)
{
	UGPWidgetComponent* WidgetComp = CreateDefaultSubobject<UGPWidgetComponent>(*Name);
	WidgetComp->SetupAttachment(GetCharacterMesh());
	WidgetComp->SetComponent(Location, Size);


	TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(nullptr, *WidgetPath);
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[CreateWidgetComponent] Failed to load widget class at path: %s"), *WidgetPath);
		return nullptr;
	}

	WidgetComp->SetWidgetClass(WidgetClass);
	WidgetComp->InitWidget();

	OutUserWidget = WidgetComp->GetUserWidgetObject();

	if (!OutUserWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[CreateWidgetComponent] Failed to create UserWidget at path: %s"), *WidgetPath);
	}

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

void AGPCharacterBase::ProcessHitHardCommand()
{
	UAnimInstance* AnimInstance = GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || !HitHardMontage)
		return;

	if (AnimInstance->Montage_IsPlaying(HitHardMontage))
		return;

	if (HitHardEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			HitHardEffect,
			GetCharacterMesh(),
			FName(TEXT("WeaponSocket")),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale,
			true);
	}

	bIsUsingSkill = true;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &AGPCharacterBase::OnSkillMontageEnded);
	AnimInstance->Montage_Play(HitHardMontage, 2.f);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, HitHardMontage);
}

void AGPCharacterBase::ProcessClashCommand()
{
	UAnimInstance* AnimInstance = GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || !ClashMontage)
		return;

	if (AnimInstance->Montage_IsPlaying(ClashMontage))
		return;


	if (ClashEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			ClashEffect,
			GetCharacterMesh(),
			FName(TEXT("WeaponSocket")),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale,
			true);
	}

	bIsUsingSkill = true;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &AGPCharacterBase::OnSkillMontageEnded);
	AnimInstance->Montage_Play(ClashMontage, 2.f);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, ClashMontage);
}

void AGPCharacterBase::ProcessWhirlwindCommand()
{
	UAnimInstance* AnimInstance = GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || !WhirlwindMontage)
		return;

	if (AnimInstance->Montage_IsPlaying(WhirlwindMontage))
		return;

	if (WhirlwindEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			WhirlwindEffect,
			GetCharacterMesh(),
			FName(TEXT("WeaponSocket")),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale,
			true);
	}

	bIsUsingSkill = true;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &AGPCharacterBase::OnSkillMontageEnded);
	AnimInstance->Montage_Play(WhirlwindMontage, 2.f);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, WhirlwindMontage);
}

void AGPCharacterBase::ProcessThrowingCommand()
{
	UAnimInstance* AnimInstance = GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || !ThrowingMontage)
		return;

	if (AnimInstance->Montage_IsPlaying(ThrowingMontage))
		return;

	if (ProjectileEffectClass)
	{
		FVector MuzzleLocation = GetMesh()->GetSocketLocation(FName("WeaponSocket"));
		MuzzleLocation.Z += 80.f;
		MuzzleLocation.Y -= 30.f;

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (!PC) return;

		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector TraceStart = CameraLocation;
		FVector TraceEnd = TraceStart + (CameraRotation.Vector() * 10000.f);
		FHitResult Hit;
		FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(ProjectileTrace), true, this);

		FVector TargetPoint = TraceEnd;
		if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
		{
			TargetPoint = Hit.ImpactPoint;
		}

		FVector AdjustedFireDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();
		FRotator AdjustedFireRotation = AdjustedFireDirection.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(
			ProjectileEffectClass,
			MuzzleLocation,
			AdjustedFireRotation,
			SpawnParams
		);
	}

	bIsUsingSkill = true;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &AGPCharacterBase::OnSkillMontageEnded);
	AnimInstance->Montage_Play(ThrowingMontage, 2.f);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, ThrowingMontage);
}

void AGPCharacterBase::ProcessFThrowingCommand()
{
	UAnimInstance* AnimInstance = GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || !FThrowingMontage)
		return;

	if (AnimInstance->Montage_IsPlaying(FThrowingMontage))
		return;

	if (ProjectileEffectClass)
	{
		FVector MuzzleLocation = GetMesh()->GetSocketLocation(FName("WeaponSocket"));
		MuzzleLocation.Z += 80.f;
		MuzzleLocation.Y -= 30.f;

		APlayerController* PC = Cast<APlayerController>(GetController());
		if (!PC) return;

		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector TraceStart = CameraLocation;
		FVector TraceEnd = TraceStart + (CameraRotation.Vector() * 10000.f);
		FHitResult Hit;
		FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(ProjectileTrace), true, this);

		FVector TargetPoint = TraceEnd;
		if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
		{
			TargetPoint = Hit.ImpactPoint;
		}

		FVector CenterDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();
		FRotator CenterRotation = CenterDirection.Rotation();

		const int32 NumProjectiles = 5;
		const float SpreadAngle = 1.2f; 

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		for (int32 i = 0; i < NumProjectiles; ++i)
		{
			int32 OffsetFromCenter = i - (NumProjectiles / 2);

			FRotator SpreadRotation = CenterRotation;
			SpreadRotation.Yaw += OffsetFromCenter * SpreadAngle;

			GetWorld()->SpawnActor<AActor>(
				ProjectileEffectClass,
				MuzzleLocation,
				SpreadRotation,
				SpawnParams
			);
		}
	}

	bIsUsingSkill = true;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &AGPCharacterBase::OnSkillMontageEnded);
	AnimInstance->Montage_Play(FThrowingMontage, 2.f);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, FThrowingMontage);
}

void AGPCharacterBase::ProcessAngerCommand()
{
	UAnimInstance* AnimInstance = GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || !AngerMontage)
		return;

	if (AnimInstance->Montage_IsPlaying(AngerMontage))
		return;

	if (AngerEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			AngerEffect,
			GetCharacterMesh(),
			FName(TEXT("WeaponSocket")),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale,
			true);
	}

	bIsUsingSkill = true;

	FOnMontageEnded MontageEndedDelegate;
	MontageEndedDelegate.BindUObject(this, &AGPCharacterBase::OnSkillMontageEnded);
	AnimInstance->Montage_Play(AngerMontage, 2.f);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AngerMontage);
}

void AGPCharacterBase::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HitHardMontage || Montage == ClashMontage || Montage == WhirlwindMontage ||
		Montage == ThrowingMontage || Montage == FThrowingMontage || Montage == AngerMontage)
	{
		bIsUsingSkill = false;

		if (CharacterInfo.HasState(STATE_SKILL_Q) && (Montage == HitHardMontage || Montage == ThrowingMontage))
		{
			CharacterInfo.RemoveState(STATE_SKILL_Q);
		}

		if (CharacterInfo.HasState(STATE_SKILL_E) && (Montage == ClashMontage || Montage == FThrowingMontage))
		{
			CharacterInfo.RemoveState(STATE_SKILL_E);
		}

		if (CharacterInfo.HasState(STATE_SKILL_R) && (Montage == WhirlwindMontage || Montage == AngerMontage))
		{
			CharacterInfo.RemoveState(STATE_SKILL_R);
		}
	}
}
