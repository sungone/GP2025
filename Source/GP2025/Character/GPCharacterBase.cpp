

#include "Character/GPCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Network/GPGameInstance.h"
#include "Character/GPCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "Physics/GPCollision.h"
#include "Engine/DamageEvents.h"
#include "CharacterStat/GPCharacterStatComponent.h"
#include "UI/GPWidgetComponent.h"
#include "UI/GPHpBarWidget.h"

// Sets default values
AGPCharacterBase::AGPCharacterBase()
{

	// ���̷�Ż �Ž� , �ִϸ��̼� �������Ʈ , �ִϸ��̼� ��Ÿ�� ����!!
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/temporaryAssets/WomanPlayer/Woman.Woman'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animation/GunnerAnimation/ABP_Gunner.ABP_Gunner_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AutoAttackMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/GunnerAnimation/AM_GunnerAttack.AM_GunnerAttack'"));
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
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_GPCAPSULE);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	{
		PlayerInfo.AddState(STATE_IDLE);
		PlayerInfo.State = 1; // IDLE
	}

	// Character Control ����
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

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> MouseMonsterDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterControl/GPC_MouseMonster.GPC_MouseMonster'"));
	if (MouseMonsterDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Mouse, MouseMonsterDataRef.Object);
	}

	// Stat Component
	Stat = CreateDefaultSubobject<UGPCharacterStatComponent>(TEXT("Stat"));

	// Widget Component
	HpBar = CreateDefaultSubobject<UGPWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/UI/WBP_CharacterHpBar.WBP_CharacterHpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.f , 15.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AGPCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �� �÷��̾��� ��ġ�� �����ϴ� ���̸� return
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (GameInstance && GameInstance->MyPlayer == this)
		return;

	// �����̸� return
	if (GameInstance->MonsterClass)
		return;

	/// Other Client ���� ��� ����ȭ ///
	if (PlayerInfo.HasState(STATE_AUTOATTACK) && bIsAutoAttacking == false)
	{
		ProcessAutoAttackCommand();
		return;
	}

	/// �̲������� ���� �ذ��ϱ� ///
	{
		if (PlayerInfo.Speed < 200.f)
		{
			PlayerInfo.Speed = 300.f;
		}
	}

	/// Other Client ��ġ �� ȸ�� ����ȭ ///
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

	/// Ohter Client �ӵ� ����ȭ ///
	GetCharacterMovement()->Velocity = MoveDir * Speed;

	/// Other Client ���� ����ȭ ///
	if (PlayerInfo.HasState(STATE_JUMP))
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	else if (!PlayerInfo.HasState(STATE_JUMP) && GetActorLocation().Z < 150.f)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void AGPCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// Stat->OnHpZero.AddUObject(this , &AGPCharacterBase::)
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

	// ���̷�Ż �޽�
	GetMesh()->SetSkeletalMesh(CharacterControlData->SkeletalMesh);

	// �ִϸ��̼� �������Ʈ
	GetMesh()->SetAnimInstanceClass(CharacterControlData->AnimBlueprint);

	// �ִϸ��̼� ��Ÿ��
	AutoAttackActionMontage = CharacterControlData->AnimMontage;
}

void AGPCharacterBase::AttackHitCheck()
{
	FHitResult OutHitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
	
	const float AttackRange = 40.f;
	const float AttackRadius = 50.f;
	const float AttackDamage = Stat->GetDamage();

	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;

	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, CCHANNEL_GPACTION, FCollisionShape::MakeSphere(AttackRadius), Params);
	if (HitDetected)
	{
		FDamageEvent DamageEvent;
		OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}

#if ENABLE_DRAW_DEBUG

	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.f);
#endif
}

float AGPCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// Stat->ApplyDamage(DamageAmount);

	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance) 
		return DamageAmount;

	if (this == GameInstance->MyPlayer) // �������� ������ �÷��̾�(��)���
	{
		UE_LOG(LogTemp, Log, TEXT("Attacked Player!!!!!!!!!!"));
		GameInstance->SendHitPacket(true , DamageAmount);
	}
	else if (this->IsA(GameInstance->MonsterClass)) // �������� ������ ���Ͷ��
	{
		UE_LOG(LogTemp, Log, TEXT("Attacked Monster!!!!!!!!!"));
		GameInstance->SendHitPacket(false , DamageAmount);
	}
	
	return DamageAmount;
}

void AGPCharacterBase::SetupCharacterWidget(UGPUserWidget* InUserWidget)
{
	UGPHpBarWidget* HpBarWidget = Cast<UGPHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UGPHpBarWidget::UpdateHpBar);
	}
}


