

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

#include <random>
static std::random_device rd;
static std::mt19937 gen(rd());

DEFINE_LOG_CATEGORY(LogGPCharacter);
namespace
{
	template <typename T>
	T* LoadAsset(const FString& Path)
	{
		static ConstructorHelpers::FObjectFinder<T> AssetRef(*Path);
		return AssetRef.Object;
	}

	template <typename T>
	TSubclassOf<T> LoadClass(const FString& Path)
	{
		static ConstructorHelpers::FClassFinder<T> ClassRef(*Path);
		return ClassRef.Class;
	}
}
AGPCharacterBase::AGPCharacterBase()
{
	// 캐릭터 스켈레탈 메시
	GetMesh()->SetSkeletalMesh(LoadAsset<USkeletalMesh>(TEXT("/Game/qudgus/chracter/mesh/main_man.main_man")));

	// 캐릭터 애니메이션 블루프린트
	GetMesh()->SetAnimInstanceClass(LoadClass<UAnimInstance>(TEXT("/Game/Animation/P_Warrior/ABP_Warrior.ABP_Warrior_C")));

	// 캐릭터 애니메이션 몽타주
	AttackActionMontage = LoadAsset<UAnimMontage>(TEXT("/Game/Animation/P_Warrior/AM_Attack.AM_Attack"));
	DeadMontage = LoadAsset<UAnimMontage>(TEXT("/Game/Animation/P_Warrior/AM_Dead.AM_Dead"));

	// 폰 회전 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캡슐 컴포넌트 설정
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_GPCAPSULE);

	// 무브먼트 컴포넌트 설정
	auto* MovementComp = GetCharacterMovement();
	MovementComp->bOrientRotationToMovement = true;
	MovementComp->RotationRate = FRotator(0.f, 500.f, 0.f);
	MovementComp->JumpZVelocity = 300.f;
	MovementComp->AirControl = 0.35f;
	MovementComp->GravityScale = 1.f;
	MovementComp->MaxWalkSpeed = 300.f;
	MovementComp->MinAnalogWalkSpeed = 20.f;
	MovementComp->BrakingDecelerationWalking = 2000.f;

	// 매쉬 설정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// 캐릭터 타입 설정
	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> WarriorDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Warrior.GPC_Warrior'"));
	if (WarriorDataRef.Object)
	{
		CharacterTypeManager.Add(ECharacterType::P_WARRIOR, WarriorDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> GunnerDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Gunner.GPC_Gunner'"));
	if (GunnerDataRef.Object)
	{
		CharacterTypeManager.Add(ECharacterType::P_GUNNER, GunnerDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> BubbleTeaDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_BubbleTea.GPC_BubbleTea'"));
	if (BubbleTeaDataRef.Object)
	{
		CharacterTypeManager.Add(ECharacterType::M_BUBBLETEA, BubbleTeaDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> EnergydrinkDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_EnergyDrink.GPC_EnergyDrink'"));
	if (EnergydrinkDataRef.Object)
	{
		CharacterTypeManager.Add(ECharacterType::M_ENERGYDRINK, EnergydrinkDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> CoffeeDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Coffee.GPC_Coffee'"));
	if (CoffeeDataRef.Object)
	{
		CharacterTypeManager.Add(ECharacterType::M_COFFEE, CoffeeDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> MouseDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_Mouse.GPC_Mouse'"));
	if (MouseDataRef.Object)
	{
		CharacterTypeManager.Add(ECharacterType::M_MOUSE, MouseDataRef.Object);
	}

	// Widget Component
	HpBar = CreateWidgetComponent(TEXT("HpWidget"), TEXT("/Game/UI/WBP_CharacterHpBar.WBP_CharacterHpBar_C"), FVector(0.f, 0.f, 300.f), FVector2D(150.f, 15.f));
	ExpBar = CreateWidgetComponent(TEXT("ExpWidget"), TEXT("/Game/UI/WBP_ExpBar.WBP_ExpBar_C"), FVector(0.f, 0.f, 308.f), FVector2D(150.f, 15.f));
	LevelText = CreateWidgetComponent(TEXT("LevelWidget"), TEXT("/Game/UI/WBP_LevelText.WBP_LevelText_C"), FVector(0.f, 0.f, 350.f), FVector2D(40.f, 10.f));

	// Item Actions
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AGPCharacterBase::EquipHelmet)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AGPCharacterBase::EquipChest)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AGPCharacterBase::DrinkPotion)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AGPCharacterBase::AddExp)));

	// Weapon Component
	Chest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest"));
	Chest->SetupAttachment(GetMesh(), TEXT("ChestSocket"));

	Helmet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet"));
	Helmet->SetupAttachment(GetMesh(), TEXT("HelmetSocket"));

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
	bool bHpChanged = CharacterInfo.Stats.Hp != CharacterInfo_.Stats.Hp;
	bool bExpChanged = CharacterInfo.Stats.Exp != CharacterInfo_.Stats.Exp;
	bool bLevelChanged = CharacterInfo.Stats.Level != CharacterInfo_.Stats.Level;

	CharacterInfo = CharacterInfo_;

	if (bHpChanged)
	{
		OnHpChanged.Broadcast(CharacterInfo.Stats.Hp / CharacterInfo.Stats.MaxHp);
	}
	if (bExpChanged)
	{
		OnExpChanged.Broadcast(CharacterInfo.Stats.Exp / CharacterInfo.Stats.MaxExp);
	}
	if (bLevelChanged)
	{
		OnLevelChanged.Broadcast(CharacterInfo.Stats.Level);
	}
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

	GetMesh()->SetSkeletalMesh(CharacterData->SkeletalMesh);
	GetMesh()->SetAnimInstanceClass(CharacterData->AnimBlueprint);

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

	const float AttackRange = CharacterInfo.AttackRange;
	const float AttackRadius = CharacterInfo.AttackRadius;

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(
		OutHitResult, Start, End, FQuat::Identity, CCHANNEL_GPACTION,
		FCollisionShape::MakeSphere(AttackRadius), Params);

	if (HitDetected)
	{
		AActor* HitActor = OutHitResult.GetActor();
		if (!HitActor) return;

		AGPCharacterBase* TargetCharacter = Cast<AGPCharacterBase>(HitActor);
		if (!TargetCharacter) return;

		UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
		if (GameInstance && this == GameInstance->MyPlayer)
		{
			GameInstance->SendPlayerAttackPacket(TargetCharacter->CharacterInfo.ID);
		}
	}

#if ENABLE_DRAW_DEBUG
	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius,
		FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(),
		DrawColor, false, 5.f);
#endif
}

UGPWidgetComponent* AGPCharacterBase::CreateWidgetComponent(const FString& Name, const FString& WidgetPath, FVector Location, FVector2D Size)
{
	UGPWidgetComponent* WidgetComp = CreateDefaultSubobject<UGPWidgetComponent>(*Name);
	WidgetComp->SetupAttachment(GetMesh());
	WidgetComp->SetRelativeLocation(Location);
	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComp->SetPivot(FVector2D(0.5f, 0.5f));
	WidgetComp->SetOwnerNoSee(true);
	WidgetComp->SetDrawSize(Size);
	WidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ConstructorHelpers::FClassFinder<UUserWidget> WidgetRef(*WidgetPath);
	if (WidgetRef.Class)
	{
		WidgetComp->SetWidgetClass(WidgetRef.Class);
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
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
	}
}

void AGPCharacterBase::AddExp(UGPItemData* InItemData)
{
	UE_LOG(LogGPCharacter, Log, TEXT("Add Exp"));
}
