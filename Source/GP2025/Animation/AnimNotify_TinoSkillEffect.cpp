// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_TinoSkillEffect.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

void UAnimNotify_TinoSkillEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !ProjectileEffectClass) return;

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	FTransform SocketTransform = MeshComp->GetSocketTransform(FName("MuzzleSocket"));
	FVector MuzzleLocation = MeshComp->GetSocketLocation(FName("MuzzleSocket")) + MuzzleOffset;

	// ������ ���� �������� �߻� (���ʹ� �÷��̾� ������ �ƴ϶� AI LookAt���� �ذ�)
	FVector ForwardDir = SocketTransform.GetUnitAxis(EAxis::Z);
	FRotator FireRotation = ForwardDir.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	OwnerActor->GetWorld()->SpawnActor<AActor>(
		ProjectileEffectClass,
		MuzzleLocation,
		FireRotation,
		SpawnParams
	);
}
