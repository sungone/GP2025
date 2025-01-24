

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

AGPCharacterBase::AGPCharacterBase()
{

	// ĳ���� ���̷�Ż �Ž� 
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/temporaryAssets/WomanPlayer/Woman.Woman'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// ĳ���� �ִϸ��̼� �������Ʈ
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animation/GunnerAnimation/ABP_Gunner.ABP_Gunner_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	// ĳ���� �ִϸ��̼� ��Ÿ��
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AutoAttackMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/GunnerAnimation/AM_GunnerAttack.AM_GunnerAttack'"));
	if (AutoAttackMontageRef.Object)
	{
		AutoAttackActionMontage = AutoAttackMontageRef.Object;
	}

	// �� ȸ���� ��Ʈ�ѷ� ȸ���� �Ȱ��� ���
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ĸ�� ������Ʈ ����
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_GPCAPSULE);

	// �����Ʈ ������Ʈ ����
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// �Ž� ����
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// CharacterInfo ����
	{
		CharacterInfo.AddState(STATE_IDLE);
		CharacterInfo.State = 1; // IDLE
	}

	// ĳ���� Ÿ�� ����
	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> WarriorDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterControl/GPC_Warrior.GPC_Warrior'"));
	if (WarriorDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterType::P_WARRIOR, WarriorDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> GunnerDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterControl/GPC_Gunner.GPC_Gunner'"));
	if (GunnerDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterType::P_GUNNER, GunnerDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> MouseMonsterDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterControl/GPC_MouseMonster.GPC_MouseMonster'"));
	if (MouseMonsterDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterType::M_MOUSE, MouseMonsterDataRef.Object);
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

	//// �����̸� return
	//if (this->IsA(GameInstance->MonsterClass))
	//	return;

	/// Other Client ���� ��� ����ȭ ///
	if (CharacterInfo.HasState(STATE_AUTOATTACK) && bIsAutoAttacking == false)
	{
		ProcessAutoAttackCommand();
		return;
	}

	/// �̲������� ���� �ذ��ϱ� ///
	{
		if (CharacterInfo.Speed < 200.f)
		{
			CharacterInfo.Speed = 300.f;
		}
	}

	/// Other Client ��ġ �� ȸ�� ����ȭ ///
	FVector Location = GetActorLocation();
	FVector DestLocation = FVector(CharacterInfo.X, CharacterInfo.Y, CharacterInfo.Z);
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

	/// Ohter Client �ӵ� ����ȭ ///
	GetCharacterMovement()->Velocity = MoveDir * Speed;

	/// Other Client ���� ����ȭ ///
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

void AGPCharacterBase::SetCharacterInfoFromServer(FInfoData& CharacterInfo_)
{
	CharacterInfo = CharacterInfo_;
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

		if (CharacterInfo.HasState(STATE_AUTOATTACK))
		{
			CharacterInfo.RemoveState(STATE_AUTOATTACK);
		}
	}
}

void AGPCharacterBase::SetCharacterControlData(const UGPCharacterControlData* CharacterControlData)
{
	// �� 
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// ĳ���� �����Ʈ 
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

void AGPCharacterBase::SetCharacterControl(ECharacterType NewCharacterControlType)
{
	UGPCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	CurrentCharacterType = NewCharacterControlType;
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

	AGPCharacterBase* AttackerCharacter = CastChecked<AGPCharacterBase>(DamageCauser);

	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance) return DamageAmount;

	bool bIsPlayer = (AttackerCharacter == GameInstance->MyPlayer);
	
	if(bIsPlayer)
		GameInstance->SendPlayerAttackPacket(this->CharacterInfo);
	else
	{
		//todo: send mons atk pkt
	
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


