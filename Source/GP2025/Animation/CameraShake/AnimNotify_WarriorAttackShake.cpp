// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CameraShake/AnimNotify_WarriorAttackShake.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"

void UAnimNotify_WarriorAttackShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    AActor* OwnerActor = MeshComp->GetOwner();
    if (!OwnerActor) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(OwnerActor->GetWorld(), 0);
    if (PC && CameraShakeClass)
    {
        PC->ClientStartCameraShake(CameraShakeClass, Scale);
    }
}
