

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

	// 캐릭터 스켈레탈 매쉬 
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/qudgus/chracter/mesh/main_man.main_man'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// 캐릭터 애니메이션 블루프린트
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Animation/P_Warrior/ABP_Warrior.ABP_Warrior_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	// 캐릭터 애니메이션 몽타주
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AutoAttackMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/P_Warrior/AM_Attack.AM_Attack'"));
	if (AutoAttackMontageRef.Object)
	{
		AttackActionMontage = AutoAttackMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/Animation/P_Warrior/AM_Dead.AM_Dead'"));
	if (DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// 폰 회전을 컨트롤러 회전과 똑같이 사용
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 캡슐 컴포넌트 설정
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_GPCAPSULE);

	// 무브먼트 컴포넌트 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// 매쉬 설정
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// CharacterInfo 설정
	{
		CharacterInfo.AddState(STATE_IDLE);
		CharacterInfo.State = 1; // IDLE
	}

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

	static ConstructorHelpers::FObjectFinder<UGPCharacterControlData> MouseMonsterDataRef(TEXT("/Script/GP2025.GPCharacterControlData'/Game/CharacterType/GPC_MouseMonster.GPC_MouseMonster'"));
	if (MouseMonsterDataRef.Object)
	{
		CharacterTypeManager.Add(ECharacterType::M_MOUSE, MouseMonsterDataRef.Object);
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
	EquipItemFromDataAsset(CharacterTypeManager[CurrentCharacterType]);

// 플레이어 바지 입히기
}

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 내 플레이어의 위치를 설정하는 것이면 return
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (GameInstance && GameInstance->MyPlayer == this)
		return;

	//// 몬스터이면 return
	//if (this->IsA(GameInstance->MonsterClass))
	//	return;

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

void AGPCharacterBase::SetCharacterInfoFromServer(FInfoData& CharacterInfo_)
{
	CharacterInfo = CharacterInfo_;
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
	AnimInstance->Montage_Play(AttackActionMontage , 1.f);
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
	// 폰 
	bUseControllerRotationYaw = CharacterData->bUseControllerRotationYaw;

	// 캐릭터 무브먼트 
	GetCharacterMovement()->bOrientRotationToMovement = CharacterData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterData->RotationRate;

	// 스켈레탈 메시
	GetMesh()->SetSkeletalMesh(CharacterData->SkeletalMesh);

	// 애니메이션 블루프린트
	GetMesh()->SetAnimInstanceClass(CharacterData->AnimBlueprint);

	// 애니메이션 몽타주 (Attack)
	AttackActionMontage = CharacterData->AttackAnimMontage;

	// 애니메이션 몽타주 (Dead)
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
	
	const float AttackRange = 40.f;
	const float AttackRadius = 50.f;
	const float AttackDamage = CharacterInfo.Damage;

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

	AGPCharacterBase* AttackerCharacter = CastChecked<AGPCharacterBase>(DamageCauser);

	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance) return DamageAmount;

	bool bIsPlayer = (AttackerCharacter == GameInstance->MyPlayer);
	
	if(bIsPlayer)
		GameInstance->SendPlayerAttackPacket(this->CharacterInfo);
	else
	{
		//todo: send mons atk pkt
		Stat->ApplyDamage(DamageAmount);

		if (Stat->GetCurrentHp() <= 0)
		{
			SetDead();
		}
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

void AGPCharacterBase::EquipHelmet(USkeletalMesh* HelmetMesh)
{
	if (!GetMesh() || !HelmetMesh) return;

	if (!HelmetMeshComp)
	{
		HelmetMeshComp = NewObject<USkeletalMeshComponent>(this);
		HelmetMeshComp->RegisterComponent();
		HelmetMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("HelmetSocket"));
	}

	HelmetMeshComp->SetSkeletalMesh(HelmetMesh);
}

void AGPCharacterBase::UnequipHelmet()
{
	if (HelmetMeshComp)
	{
		HelmetMeshComp->DestroyComponent();
		HelmetMeshComp = nullptr;
	}
}

void AGPCharacterBase::EquipChest(USkeletalMesh* ChestMesh)
{
	if (!GetMesh() || !ChestMesh) return;

	if (!ChestMeshComp)
	{
		ChestMeshComp = NewObject<USkeletalMeshComponent>(this);
		ChestMeshComp->RegisterComponent();
		ChestMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ChestSocket"));
	}
	ChestMeshComp->SetSkeletalMesh(ChestMesh);
}

void AGPCharacterBase::UnequipChest()
{
	if (ChestMeshComp)
	{
		ChestMeshComp->DestroyComponent();
		ChestMeshComp = nullptr;
	}
}

void AGPCharacterBase::EquipSword(USkeletalMesh* SwordMesh)
{
	 
}

void AGPCharacterBase::UnequipSword()
{

}

void AGPCharacterBase::EquipGun(USkeletalMesh* GunMesh)
{

}

void AGPCharacterBase::UnequipGun()
{
}

void AGPCharacterBase::EquipPants(USkeletalMesh* PantsMesh)
{
	if (!GetMesh() || !PantsMesh) return;

	if (!PantsMeshComp)
	{
		PantsMeshComp = NewObject<USkeletalMeshComponent>(this);
		PantsMeshComp->RegisterComponent();
		PantsMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("PantsSocket"));
	}

	PantsMeshComp->SetSkeletalMesh(PantsMesh);
}

void AGPCharacterBase::UnequipPants()
{
	if (PantsMeshComp)
	{
		PantsMeshComp->DestroyComponent();
		PantsMeshComp = nullptr;
	}
}

void AGPCharacterBase::EquipItemFromDataAsset(UGPCharacterControlData* CharacterData)
{
	if (!CharacterData) return;

	if (CharacterData->HelmetMesh)
	{
		EquipHelmet(CharacterData->HelmetMesh);
	}
	else
	{
		UnequipHelmet();
	}

	if (CharacterData->ChestMesh)
	{
		EquipChest(CharacterData->ChestMesh);
	}
	else
	{
		UnequipChest();
	}

	if (CharacterData->SwordMesh)
	{
		EquipSword(CharacterData->SwordMesh);
	}
	else
	{
		UnequipSword();
	}

	if (CharacterData->GunMesh)
	{
		EquipGun(CharacterData->GunMesh);
	}
	else
	{
		UnequipGun();
	}

	if (CharacterData->PantsMesh)
	{
		EquipPants(CharacterData->PantsMesh);
	}
	else
	{
		UnequipPants();
	}

	UE_LOG(LogTemp, Log, TEXT("Equipped!!!"));
}


