// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPCharacterCombatHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "Animation/AnimInstance.h"
#include "Network/GPNetworkManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/Modules/GPMyplayerSoundManager.h"


void UGPCharacterCombatHandler::Initialize(AGPCharacterBase* InOwner)
{
	Owner = InOwner;
}

void UGPCharacterCombatHandler::PlayAutoAttackMontage()
{
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] Owner is null. Cannot play auto attack montage."));
		return;
	}

	UAnimMontage* MontageToPlay = nullptr;

	bool bHasWeapon = Owner->HasWeaponEquipped();

	if (bHasWeapon)
	{
		MontageToPlay = AttackMontage;
		UE_LOG(LogTemp, Log, TEXT("[Combat] Selected AttackMontage (with weapon)."));
	}
	else
	{
		MontageToPlay = AttackWithoutWeaponMontage;
		UE_LOG(LogTemp, Log, TEXT("[Combat] Selected AttackWithoutWeaponMontage (unarmed)."));
	}

	if (!MontageToPlay)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] Selected Montage is null. Cannot play animation."));
		return;
	}

	UAnimInstance* AnimInstance = Owner->GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] AnimInstance is null. Cannot play montage."));
		return;
	}

	if (AnimInstance->Montage_IsPlaying(MontageToPlay))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Combat] Montage is already playing. Skipping playback."));
		return;
	}

	// Sound
	if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(Owner))
	{
		if (MyPlayer->SoundManager)
		{
			USoundBase* AttackSound = nullptr;
			float SoundPlayRate = 1.f;
			if (!bHasWeapon)
			{
				// 맨손 공격일 경우
				AttackSound = MyPlayer->SoundManager->PlayerPunchSound; 
			}
			else if (MyPlayer->bIsGunnerCharacter())
			{
				AttackSound = MyPlayer->SoundManager->GunnerAttackSound;
			}
			else
			{
				AttackSound = MyPlayer->SoundManager->WarriorAttackSound;
				SoundPlayRate = 0.8f; // Warrior는 약간 느리게
			}

			if (AttackSound)
			{
				MyPlayer->SoundManager->PlaySFX(AttackSound, 1.f, SoundPlayRate); // Volume, Pitch
			}
		}
	}

	bIsAutoAttacking = true;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UGPCharacterCombatHandler::OnAutoAttackMontageEnded);

	float Result = AnimInstance->Montage_Play(MontageToPlay, PlayRate);
	if (Result == 0.f)
	{
		UE_LOG(LogTemp, Error, TEXT("[Combat] Montage_Play failed. PlayRate: %f, Montage: %s"), PlayRate, *MontageToPlay->GetName());
		return;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Combat] Montage_Play succeeded. Playing: %s at rate %f"), *MontageToPlay->GetName(), PlayRate);
	}

	AnimInstance->Montage_SetEndDelegate(EndDelegate, MontageToPlay);

	// Fail-safe timer
	float MontageDuration = MontageToPlay->GetPlayLength();
	float AdjustedDuration = MontageDuration / PlayRate;
	Owner->GetWorldTimerManager().SetTimer(
		AutoAttackFailSafeHandle,
		[this]()
		{
			if (bIsAutoAttacking)
			{
				UE_LOG(LogTemp, Error, TEXT("[Combat] Failsafe: Montage didn't end properly. Cleaning up manually."));
				bIsAutoAttacking = false;
				Owner->CharacterInfo.RemoveState(STATE_AUTOATTACK);
			}
		},
		AdjustedDuration + 0.2f,
		false
	);
}

void UGPCharacterCombatHandler::OnAutoAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Owner || Montage != AttackMontage) return;
	bIsAutoAttacking = false;
	Owner->CharacterInfo.RemoveState(STATE_AUTOATTACK);

	// 실패 방지용 코드 타이머 제거
	if (Owner->GetWorldTimerManager().IsTimerActive(AutoAttackFailSafeHandle))
	{
		Owner->GetWorldTimerManager().ClearTimer(AutoAttackFailSafeHandle);
	}
}

void UGPCharacterCombatHandler::HandleDeath()
{
	if (!Owner) return;

	Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	Owner->SetActorEnableCollision(false);

	PlayDeadAnimation();

	FTimerHandle DeadTimerHandle;
	AGPCharacterBase* LocalOwner = Owner;

	if (AGPCharacterMyplayer* LocalMyPlayer = Cast<AGPCharacterMyplayer>(LocalOwner))
	{
		LocalMyPlayer->SetActorHiddenInGame(true);
		return;
	}
	
	Owner->GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([LocalOwner]()
		{
			if (LocalOwner)
			{
				LocalOwner->Destroy();
			}
		}), DeadEventDelayTime, false);
}

void UGPCharacterCombatHandler::PlayDeadAnimation()
{
	if (!Owner || !DeadMontage) return;

	UAnimInstance* AnimInstance = Owner->GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	AnimInstance->StopAllMontages(0.f);
	AnimInstance->Montage_Play(DeadMontage, 1.f);
}

void UGPCharacterCombatHandler::SetDeadEventDelay(float Delay)
{
	DeadEventDelayTime = Delay;
}

void UGPCharacterCombatHandler::PlayQSkillMontage()
{
	PlaySkillMontage(QSkillMontage);

}

void UGPCharacterCombatHandler::PlayESkillMontage()
{
	PlaySkillMontage(ESkillMontage);
}

void UGPCharacterCombatHandler::PlayRSkillMontage()
{
	PlaySkillMontage(RSkillMontage);
}

void UGPCharacterCombatHandler::StartDash()
{
	if (!Owner) return;

	FVector ForwardDirection = Owner->GetActorForwardVector();
	DashStartLocation = Owner->GetActorLocation();
	DashEndLocation = DashStartLocation + (ForwardDirection * DashDistance);

	// 이동을 막고, 이동 모드 변경
	Owner->GetCharacterMovement()->DisableMovement();

	// E 스킬 몽타지 재생
	PlaySkillMontage(ESkillMontage);

	// 일정 시간 후에 돌진 종료 및 스킬 애니메이션 재생
	Owner->GetWorld()->GetTimerManager().SetTimer(
		DashTimerHandle,
		this,
		&UGPCharacterCombatHandler::UpdateDash,
		0.01f, 
		true
	);
}

void UGPCharacterCombatHandler::FinishDash()
{
	if (!Owner) return;

	//// 이동 모드 복원
	Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	UE_LOG(LogTemp, Log, TEXT("Dash finished, playing skill montage"));


	bIsDashing = false;
	DashElapsedTime = 0.0f;

	// E 스킬 몽타지 재생
	//PlaySkillMontage(ESkillMontage);

	// 공격 패킷 전송 (데미지 처리)
	AGPCharacterMyplayer* LocalMyPlayer = Cast<AGPCharacterMyplayer>(Owner);
	if (LocalMyPlayer && LocalMyPlayer->NetMgr)
	{
		FVector Location = LocalMyPlayer->GetActorLocation();
		float Yaw = LocalMyPlayer->GetControlRotation().Yaw;

		LocalMyPlayer->NetMgr->SendMyUseSkill(ESkillGroup::Clash, Yaw, Location);
	}
}

void UGPCharacterCombatHandler::UpdateDash()
{
	if (!Owner) return;

	DashElapsedTime += 0.01f;

	float Alpha = FMath::Clamp(DashElapsedTime / DashDuration, 0.0f, 1.0f);

	FVector NewLocation = FMath::Lerp(DashStartLocation, DashEndLocation, Alpha);
	Owner->SetActorLocation(NewLocation);

	if (Alpha >= 1.0f)
	{
		Owner->GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle);
		FinishDash();
	}
}

void UGPCharacterCombatHandler::PlayMultiHitSkill(int32 HitCount, float Interval)
{
	if (!Owner) return;

	// 기존 타이머가 남아있으면 제거
	if (Owner->GetWorldTimerManager().IsTimerActive(MultiHitTimerHandle))
	{
		Owner->GetWorldTimerManager().ClearTimer(MultiHitTimerHandle);
	}

	// 초기화
	RemainingHits = HitCount;
	HitInterval = Interval;

	// 첫 번째 공격 실행
	ExecuteMultiHit();

	// 이후 타이머로 반복 실행
	Owner->GetWorldTimerManager().SetTimer(
		MultiHitTimerHandle,
		this,
		&UGPCharacterCombatHandler::ExecuteMultiHit,
		HitInterval,
		true
	);
}

void UGPCharacterCombatHandler::ExecuteMultiHit()
{
	if (RemainingHits <= 0)
	{
		// 공격이 끝났으면 타이머 해제
		Owner->GetWorldTimerManager().ClearTimer(MultiHitTimerHandle);
		return;
	}

	AGPCharacterMyplayer* LocalMyPlayer = Cast<AGPCharacterMyplayer>(Owner);
	if (!LocalMyPlayer) return;

	if (LocalMyPlayer && LocalMyPlayer->NetMgr)
	{
		FVector Location = LocalMyPlayer->GetActorLocation();
		float Yaw = LocalMyPlayer->GetControlRotation().Yaw;

		UE_LOG(LogTemp, Log, TEXT("MultiHit Attack: %d hits remaining"), RemainingHits);
		LocalMyPlayer->NetMgr->SendMyUseSkill(ESkillGroup::Whirlwind, Yaw, Location);
	}

	RemainingHits--;
}

void UGPCharacterCombatHandler::PlaySkillMontage(UAnimMontage* SkillMontage)
{
	if (!Owner || !SkillMontage) return;

	UAnimInstance* AnimInstance = Owner->GetCharacterMesh()->GetAnimInstance();
	if (!AnimInstance || AnimInstance->Montage_IsPlaying(SkillMontage)) return;

	if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(Owner))
	{
		if (MyPlayer->SoundManager)
		{
			USoundBase* SkillSound = nullptr;
			float SoundPlayRate = 1.f;
			float SoundVolume = 1.f;

			if (SkillMontage == QSkillMontage)
			{
				SkillSound = MyPlayer->bIsGunnerCharacter() ?
					MyPlayer->SoundManager->GunnerQSkillSound :
					MyPlayer->SoundManager->WarriorQSkillSound;
			}
			else if (SkillMontage == ESkillMontage)
			{
				SkillSound = MyPlayer->bIsGunnerCharacter() ?
					MyPlayer->SoundManager->GunnerESkillSound :
					MyPlayer->SoundManager->WarriorESkillSound;

				if (!MyPlayer->bIsGunnerCharacter())
				{
					SoundPlayRate = 1.2;
					SoundVolume = 1.5;
				}
			}
			else if (SkillMontage == RSkillMontage)
			{
				// Gunner는 R 스킬이 없고 Warrior만 R을 사용한다고 가정
				SkillSound = MyPlayer->SoundManager->WarriorRSkillSound;
			}

			if (SkillSound)
			{
				MyPlayer->SoundManager->PlaySFX(SkillSound , SoundPlayRate, SoundVolume);
			}
		}
	}

	bIsUsingSkill = true;

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UGPCharacterCombatHandler::OnSkillMontageEnded);
	AnimInstance->Montage_Play(SkillMontage, PlayRate);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, SkillMontage);

	float AdjustedDuration = SkillMontage->GetPlayLength() / PlayRate;
	CurrentSkillMontage = SkillMontage;

	Owner->GetWorldTimerManager().SetTimer(
		SkillFailSafeHandle,
		[this]()
		{
			if (bIsUsingSkill)
			{
				UE_LOG(LogTemp, Error, TEXT("[Combat] Failsafe: Skill Montage did not end. Cleaning up manually."));
				bIsUsingSkill = false;

				// 상태 제거 (Q/E/R 중 어떤 것인지 확인 후 제거)
				if (CurrentSkillMontage == QSkillMontage)
					Owner->CharacterInfo.RemoveState(STATE_SKILL_Q);
				else if (CurrentSkillMontage == ESkillMontage)
				{
					Owner->CharacterInfo.RemoveState(STATE_SKILL_E);
					if (Owner->CharacterInfo.CharacterType == static_cast<uint8>(Type::EPlayer::WARRIOR))
					{
						bIsDashing = false;
						Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
					}
				}
				else if (CurrentSkillMontage == RSkillMontage)
					Owner->CharacterInfo.RemoveState(STATE_SKILL_R);
			}
		},
		AdjustedDuration + 0.2f,
		false
	);
}

void UGPCharacterCombatHandler::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Owner)
		return;

	if (Montage == QSkillMontage || Montage == ESkillMontage || Montage == RSkillMontage)
	{
		bIsUsingSkill = false;

		if (Owner->CharacterInfo.HasState(STATE_SKILL_Q) && Montage == QSkillMontage)
			Owner->CharacterInfo.RemoveState(STATE_SKILL_Q);

		if (Owner->CharacterInfo.HasState(STATE_SKILL_E) && Montage == ESkillMontage)
		{
			Owner->CharacterInfo.RemoveState(STATE_SKILL_E);
			if (Owner->CharacterInfo.CharacterType == static_cast<uint8>(Type::EPlayer::WARRIOR))
			{
				Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			}
		}

		if (Owner->CharacterInfo.HasState(STATE_SKILL_R) && Montage == RSkillMontage)
			Owner->CharacterInfo.RemoveState(STATE_SKILL_R);

		if (Owner->GetWorldTimerManager().IsTimerActive(SkillFailSafeHandle))
		{
			Owner->GetWorldTimerManager().ClearTimer(SkillFailSafeHandle);
		}
	}
}

void UGPCharacterCombatHandler::SetAttackWithoutWeaponMontage(UAnimMontage* Montage)
{
	AttackWithoutWeaponMontage = Montage;
}

void UGPCharacterCombatHandler::SetAttackMontage(UAnimMontage* Montage)
{
	AttackMontage = Montage;
}

void UGPCharacterCombatHandler::SetQSkillMontage(UAnimMontage* Montage)
{
	QSkillMontage = Montage;
}

void UGPCharacterCombatHandler::SetESkillMontage(UAnimMontage* Montage)
{
	ESkillMontage = Montage;
}

void UGPCharacterCombatHandler::SetRSkillMontage(UAnimMontage* Montage)
{
	RSkillMontage = Montage;
}

void UGPCharacterCombatHandler::ApplyAttackSpeedBoost(float BoostPlayRate, float Duration)
{
	if (!Owner) return;

	if (Owner->GetWorldTimerManager().IsTimerActive(AttackSpeedResetTimer))
	{
		Owner->GetWorldTimerManager().ClearTimer(AttackSpeedResetTimer);
	}

	// 공격 속도 증가
	PlayRate = BoostPlayRate;
	UE_LOG(LogTemp, Log, TEXT("Attack Speed Boosted to %f for %f seconds"), PlayRate, Duration);

	// 일정 시간 후에 원래 속도로 복귀
	Owner->GetWorldTimerManager().SetTimer(
		AttackSpeedResetTimer,
		this,
		&UGPCharacterCombatHandler::ResetAttackSpeed,
		Duration,
		false
	);
}

void UGPCharacterCombatHandler::ResetAttackSpeed()
{
	PlayRate = DefaultPlayRate;
	UE_LOG(LogTemp, Log, TEXT("Attack Speed Reset to %f"), PlayRate);
}

void UGPCharacterCombatHandler::SetDeadMontage(UAnimMontage* Montage)
{
	DeadMontage = Montage;
}
