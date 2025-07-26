// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterMonster.h"
#include "Character/GPCharacterMyPlayer.h"
#include "Character/GPCharacterControlData.h"
#include "Character/Modules/GPCharacterUIHandler.h"
#include "Character/Modules/GPCharacterCombatHandler.h"
#include "UI/GPHpBarWidget.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "UI/GPLevelWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "UI/GPWidgetComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "UObject/ConstructorHelpers.h"

AGPCharacterMonster::AGPCharacterMonster()
{
	GetMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> HitEffectAsset(TEXT("/Game/effect/ARPGEssentials/Effects/NS_ChromaticSlash_Impact_Stab_01.NS_ChromaticSlash_Impact_Stab_01"));
	if (HitEffectAsset.Succeeded())
	{
		HitEffect = HitEffectAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DeathEffectObj(TEXT("/Game/effect/ARPGEssentials/Effects/NS_ChromaticSlash_OmniBurst.NS_ChromaticSlash_OmniBurst"));
	if (DeathEffectObj.Succeeded())
	{
		DeathEffect = DeathEffectObj.Object;
	}

	// MonsterHitSound 로드
	static ConstructorHelpers::FObjectFinder<USoundBase> HitSoundObj(TEXT("/Game/Sound/SFX/MonsterHit2.MonsterHit2"));
	if (HitSoundObj.Succeeded())
	{
		MonsterHitSound = HitSoundObj.Object;
	}

	// MonsterCriticalHitSound 로드
	static ConstructorHelpers::FObjectFinder<USoundBase> CritHitSoundObj(TEXT("/Game/Sound/SFX/MonsterHit.MonsterHit"));
	if (CritHitSoundObj.Succeeded())
	{
		MonsterCriticalHitSound = CritHitSoundObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> TinoHitSoundObj(TEXT("/Game/Sound/SFX/TinoHit.TinoHit"));
	if (TinoHitSoundObj.Succeeded())
	{
		TinoHitSound = TinoHitSoundObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> MonDeadSoundObj(TEXT("/Game/Sound/SFX/MonsterDeadSound.MonsterDeadSound"));
	if (MonDeadSoundObj.Succeeded())
	{
		MonsterDeadSound = MonDeadSoundObj.Object;
	}

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	GetCapsuleComponent()->SetupAttachment(SceneRoot);
	GetMesh()->SetupAttachment(GetCapsuleComponent());
}

void AGPCharacterMonster::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterType(CurrentCharacterType);

	// 플레이어와 충돌 설정 (카메라 시야 방해 때문에 설정)
	USkeletalMeshComponent* MyMesh = GetMesh();
	MyMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MyMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	MyMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	MyMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	MyMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AGPCharacterMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (UIHandler)
		UIHandler->UpdateWidgetVisibility();
	// Attack Sync
	HandleQSkillState();
	HandleESkillState();
	HandleRSkillState();
	HandleAutoAttackState();
}

void AGPCharacterMonster::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
	Super::SetCharacterData(CharacterControlData);
	GetMesh()->SetSkeletalMesh(CharacterControlData->SkeletalMesh);
	GetMesh()->SetAnimInstanceClass(CharacterControlData->AnimBlueprint);

	GetCapsuleComponent()->SetCapsuleHalfHeight(CharacterControlData->CapsuleHalfHeight);
	GetCapsuleComponent()->SetCapsuleRadius(CharacterControlData->CapsuleRadius);

	CombatHandler->SetMonsterHitMontage(CharacterControlData->MonsterHitAnimMontage);

	//if (CharacterControlData->bIsBoos)
	//ApplyCapsuleAndMeshScaling(CharacterControlData->CapsuleRadius, CharacterControlData->CapsuleHalfHeight);

	AdjustMeshToCapsule();
}

void AGPCharacterMonster::SetCharacterType(ECharacterType NewCharacterControlType)
{
	Super::SetCharacterType(NewCharacterControlType);
}

void AGPCharacterMonster::ApplyCapsuleAndMeshScaling(float CapsuleRadius, float CapsuleHalfHeight)
{
	const float BaseRadius = 42.f;
	const float BaseHalfHeight = 99.f;

	const float RadiusScaleRatio = CapsuleRadius / BaseRadius;
	const float HeightScaleRatio = CapsuleHalfHeight / BaseHalfHeight;

	const float FinalScale = (RadiusScaleRatio + HeightScaleRatio) * 0.5f;

	GetMesh()->SetWorldScale3D(FVector(FinalScale));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleHalfHeight));
}

void AGPCharacterMonster::AdjustMeshToCapsule()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	USkeletalMeshComponent* MyMesh = GetMesh();

	if (!Capsule || !MyMesh) return;

	const float CapsuleHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	const float CapsuleFullHeight = CapsuleHalfHeight * 2.f;

	const FBoxSphereBounds Bounds = MyMesh->GetLocalBounds();
	const float MeshTotalHeight = Bounds.BoxExtent.Z * 2.f;

	const float ScaleRatio = CapsuleFullHeight / MeshTotalHeight;
	MyMesh->SetWorldScale3D(FVector(ScaleRatio));

	MyMesh->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleHalfHeight));

	Capsule->SetRelativeLocation(FVector(0.f, 0.f, CapsuleHalfHeight));
	Capsule->SetCapsuleRadius(CharacterInfo.CollisionRadius);
}

void AGPCharacterMonster::PlayDeathEffect()
{
	if (DeathEffect)
	{
		FVector SpawnLocation = GetActorLocation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DeathEffect,
			SpawnLocation,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true,                       // bAutoDestroy
			true,                       // bAutoActivate
			ENCPoolMethod::None,        // Pooling Method
			true                        // bPreCullCheck
		);
		UE_LOG(LogTemp, Log, TEXT("DeathEffect spawned for Monster [%s]"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DeathEffect is null for Monster [%s]"), *GetName());
	}
}

void AGPCharacterMonster::PlayHitEffect()
{
	if (HitEffect)
	{
		UNiagaraComponent* EffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			HitEffect,
			GetMesh(),
			TEXT("HitSocket"),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTargetIncludingScale,
			false
		);

		if (EffectComponent)
		{
			EffectComponent->SetWorldScale3D(FVector(2.f));
			EffectComponent->Activate(true);
		}
	}
}
