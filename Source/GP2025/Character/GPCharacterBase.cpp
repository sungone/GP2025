

#include "Character/GPCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Network/GPGameInstance.h"
#include "Character/GPCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "Physics/GPCollision.h"
#include "Engine/DamageEvents.h"
#include "UI/GPWidgetComponent.h"
#include "UI/GPHpBarWidget.h"
#include "UI/GPExpBarWidget.h"
#include "UI/GPLevelWidget.h"
#include "UI/GPFloatingDamageText.h"
#include "Item/GPEquipItemData.h"
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
		MakeTuple(Type::EMonster::MOUSE, TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Mouse.GPC_Mouse'"))
	};

	LoadCharacterData(CharacterTypeManager, CharacterTypes);

	HpBar = CreateWidgetComponent(TEXT("HpWidget"), TEXT("/Game/UI/WBP_CharacterHpBar.WBP_CharacterHpBar_C"), FVector(0.f, 0.f, 300.f), FVector2D(150.f, 15.f));
	//ExpBar = CreateWidgetComponent(TEXT("ExpWidget"), TEXT("/Game/UI/WBP_ExpBar.WBP_ExpBar_C"), FVector(0.f, 0.f, 308.f), FVector2D(150.f, 15.f));
	LevelText = CreateWidgetComponent(TEXT("LevelWidget"), TEXT("/Game/UI/WBP_LevelText.WBP_LevelText_C"), FVector(0.f, 0.f, 340.f), FVector2D(40.f, 10.f));

	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AGPCharacterBase::EquipHelmet)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AGPCharacterBase::EquipChest)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AGPCharacterBase::DrinkPotion)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AGPCharacterBase::AddExp)));

	Chest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));
	Chest->SetupAttachment(GetMesh(), TEXT("ChestSocket"));
	Chest->SetCollisionProfileName(TEXT("NoCollision"));
	Chest->SetVisibility(true);

	Helmet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet"));
	Helmet->SetupAttachment(GetMesh(), TEXT("HelmetSocket"));
	Helmet->SetCollisionProfileName(TEXT("NoCollision"));
	Helmet->SetVisibility(true);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetCollisionProfileName(TEXT("NoCollision"));
	BodyMesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f) , FRotator(0.f , -90.f , 0.f));

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(BodyMesh);

	LegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegMesh"));
	LegMesh->SetupAttachment(BodyMesh);
}

void AGPCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// 런타임에 동적으로 Data Asset 로드
	UGPCharacterControlData* LoadedCharacterData = LoadObject<UGPCharacterControlData>(nullptr, TEXT("/Game/CharacterType/GPC_Warrior.GPC_Warrior"));

	if (LoadedCharacterData)
	{
		ApplyCharacterPartsFromData(LoadedCharacterData);
	}
}

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 내 플레이어의 위치를 설정하는 것이면 return
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (GameInstance && GameInstance->MyPlayer == this)
		return;

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

void AGPCharacterBase::ProcessAutoAttackCommand()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
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

	//GetMesh()->SetSkeletalMesh(CharacterData->SkeletalMesh);
	//GetMesh()->SetAnimInstanceClass(CharacterData->AnimBlueprint);

	if (CharacterData->AnimBlueprint)
	{
		BodyMesh->SetAnimInstanceClass(CharacterData->AnimBlueprint);
	}

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
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	// 공격 범위 및 반경 가져오기
	const float AttackRange = CharacterInfo.AttackRange;
	const float AttackRadius = CharacterInfo.AttackRadius;

	// 공격 시작점과 끝점 계산
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	// 스피어(구형) 충돌 검사 수행
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

			// 공격 패킷 전송 (클라이언트 본인 캐릭터만)
			UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
			if (IsValid(GameInstance) && this == GameInstance->MyPlayer)
			{
				GameInstance->SendPlayerAttackPacket(TargetCharacter->CharacterInfo.ID);
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	// 공격 범위를 캡슐 형태로 시각화
	const FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	const float CapsuleHalfHeight = AttackRange * 0.5f;
	//const FColor DrawColor = bHitDetected ? FColor::Green : FColor::Red;
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
	WidgetComp->SetupAttachment(GetMesh());
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	AnimInstance->StopAllMontages(0.f);
	AnimInstance->Montage_Play(DeadMontage, 1.f);
}

// Item System
/////////////////////////////////////////////////////////////

void AGPCharacterBase::TakeItem(UGPItemData* InItemData)
{
	if (InItemData)
	{
		TakeItemActions[(uint8)InItemData->Type].ItemDelegate.ExecuteIfBound(InItemData);
	}
}

void AGPCharacterBase::DrinkPotion(UGPItemData* InItemData)
{
	UE_LOG(LogGPCharacter, Log, TEXT("Drink Potion"));
}

void AGPCharacterBase::EquipChest(UGPItemData* InItemData)
{
	UE_LOG(LogGPCharacter, Log, TEXT("Equip Chest"));
	UGPEquipItemData* ChestItemData = Cast<UGPEquipItemData>(InItemData);
	if (ChestItemData)
	{
		Chest->SetLeaderPoseComponent(GetMesh());
		Chest->SetSkeletalMesh(ChestItemData->EquipMesh);
	}
}

void AGPCharacterBase::EquipHelmet(UGPItemData* InItemData)
{
	UE_LOG(LogGPCharacter, Log, TEXT("Equip Helmet"));
	UGPEquipItemData* HelmetItemData = Cast<UGPEquipItemData>(InItemData);
	if (HelmetItemData)
	{
		Helmet->SetSkeletalMesh(HelmetItemData->EquipMesh);
		Helmet->SetLeaderPoseComponent(GetMesh());
	}
}

void AGPCharacterBase::AddExp(UGPItemData* InItemData)
{
	UE_LOG(LogGPCharacter, Log, TEXT("Add Exp"));
}

void AGPCharacterBase::SetupMasterPose()
{
	if (BodyMesh)
	{
		if (HeadMesh)
		{
			HeadMesh->SetMasterPoseComponent(BodyMesh);
		}
		if (LegMesh)
		{
			LegMesh->SetMasterPoseComponent(BodyMesh);
		}
	}
}

void AGPCharacterBase::ApplyCharacterPartsFromData(const UGPCharacterControlData* CharacterData)
{
	if (!CharacterData) return;

	if (CharacterData->BodyMesh)
	{
		BodyMesh->SetSkeletalMesh(CharacterData->BodyMesh);
	}

	if (CharacterData->HeadMesh)
	{
		HeadMesh->SetSkeletalMesh(CharacterData->HeadMesh);
	}

	if (CharacterData->LegMesh)
	{
		LegMesh->SetSkeletalMesh(CharacterData->LegMesh);
	}

	if (CharacterData->HelmetMesh)
	{
		Helmet->SetSkeletalMesh(CharacterData->HelmetMesh);
		Helmet->AttachToComponent(HeadMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HelmetSocket"));
		Helmet->SetVisibility(true);

		Helmet->SetMasterPoseComponent(HeadMesh);
	}
	else
	{
		Helmet->SetSkeletalMesh(nullptr);
		Helmet->SetVisibility(false);
	}

	if (CharacterData->ChestMesh)
	{
		Chest->SetSkeletalMesh(CharacterData->ChestMesh);
		Chest->AttachToComponent(BodyMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("ChestSocket"));
		Chest->SetVisibility(true);

		Chest->SetMasterPoseComponent(BodyMesh);
	}
	else
	{
		Chest->SetSkeletalMesh(nullptr);
		Chest->SetVisibility(false);
	}

	SetupMasterPose();
}
