

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
	auto* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->InitCapsuleSize(42.f, 99.f);
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
			UIHandler->CreateAllWidgets();   
			UIHandler->OnBeginPlay();
		}
	}
}

void AGPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugCollisionAndAttackRadius(GetWorld(), CharacterInfo);

	if (Cast<AGPCharacterMyplayer>(this)) return;

	// Move Sync
	HandleAutoAttackState();
	HandleRemoteMovementSync(DeltaTime);
	HandleRemoteJumpSync();


}

void AGPCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AGPCharacterBase::SetCharacterInfo(FInfoData& CharacterInfo_)
{
	CharacterInfo = CharacterInfo_;

	OnHpChanged.Broadcast(CharacterInfo.Stats.Hp / CharacterInfo.Stats.MaxHp);
	OnLevelChanged.Broadcast(CharacterInfo.Stats.Level);

	if (CharacterInfo.Gold != CharacterInfo_.Gold)
	{
		OnGoldChanged.Broadcast(CharacterInfo.Gold);
	}

	if (UIHandler)
	{
		UIHandler->SetupNickNameUI();
	}
}

void AGPCharacterBase::HandleAutoAttackState()
{
	if (CharacterInfo.HasState(STATE_AUTOATTACK) && !CombatHandler->IsAutoAttacking())
	{
		CombatHandler->PlayAutoAttackMontage();
	}
}

void AGPCharacterBase::HandleRemoteMovementSync(float DeltaTime)
{
	FVector Location = GetActorLocation();
	FVector DestLocation = CharacterInfo.Pos;

	float Speed = CharacterInfo.HasState(STATE_RUN) ? 1200.f : 600.f;

	FVector MoveDir = (DestLocation - Location);
	float DistToDest = MoveDir.Length();
	MoveDir.Normalize();

	float MoveDist = FMath::Min((MoveDir * Speed * DeltaTime).Length(), DistToDest);
	FVector NextLocation = Location + MoveDir * MoveDist;

	FRotator InterpolatedRotation = FMath::RInterpTo(
		GetActorRotation(),
		FRotator(0.f, CharacterInfo.Yaw, 0.f),
		DeltaTime,
		10.0f
	);

	SetActorLocationAndRotation(NextLocation, InterpolatedRotation);

	FVector FinalVelocity = MoveDir * Speed;
	GetCharacterMovement()->Velocity = FinalVelocity;
}

void AGPCharacterBase::HandleRemoteJumpSync()
{
	if (CharacterInfo.HasState(STATE_JUMP))
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	else if (GetActorLocation().Z < 147.7f)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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
		CombatHandler->SetAttackMontage(CharacterData->AttackAnimMontage);
		CombatHandler->SetDeadMontage(CharacterData->DeadAnimMontage);

		if (CurrentCharacterType == (uint8)Type::EPlayer::GUNNER)
		{
			CombatHandler->SetQSkillMontage(CharacterData->QSkillAnimMontage);  
			CombatHandler->SetESkillMontage(CharacterData->ESkillAnimMontage);  
			CombatHandler->SetRSkillMontage(CharacterData->RSkillAnimMontage);  
		}
		else
		{
			CombatHandler->SetQSkillMontage(CharacterData->QSkillAnimMontage);  
			CombatHandler->SetESkillMontage(CharacterData->ESkillAnimMontage);  
			CombatHandler->SetRSkillMontage(CharacterData->RSkillAnimMontage);  
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