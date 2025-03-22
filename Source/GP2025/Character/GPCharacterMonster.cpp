// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterMonster.h"
#include "Character/GPCharacterControlData.h"
#include "UI/GPHpBarWidget.h"
#include "UI/GPLevelWidget.h"
#include "Kismet/GameplayStatics.h"

AGPCharacterMonster::AGPCharacterMonster()
{
	HpBar = CreateWidgetComponent(TEXT("HpWidget"), TEXT("/Game/UI/WBP_CharacterHpBar.WBP_CharacterHpBar_C"), FVector(0.f, 0.f, 300.f), FVector2D(150.f, 15.f) , HpBarWidget);
	LevelText = CreateWidgetComponent(TEXT("LevelWidget"), TEXT("/Game/UI/WBP_LevelText.WBP_LevelText_C"), FVector(0.f, 0.f, 330.f), FVector2D(40.f, 10.f) , LevelTextWidget);
}

void AGPCharacterMonster::BeginPlay()
{
	Super::BeginPlay();
	SetCharacterType(CurrentCharacterType);
}

void AGPCharacterMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateWidgetVisibility();
}

void AGPCharacterMonster::SetCharacterData(const UGPCharacterControlData* CharacterControlData)
{
	Super::SetCharacterData(CharacterControlData);
	GetMesh()->SetSkeletalMesh(CharacterControlData->SkeletalMesh);
	GetMesh()->SetAnimInstanceClass(CharacterControlData->AnimBlueprint);
}

void AGPCharacterMonster::SetCharacterType(ECharacterType NewCharacterControlType)
{
	Super::SetCharacterType(NewCharacterControlType);
}

void AGPCharacterMonster::UpdateWidgetVisibility()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController)
    {
        return;
    }

    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!PlayerPawn)
    {
        return;
    }

    float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());


    ESlateVisibility VisibilityState = (Distance > 300.f)
        ? ESlateVisibility::Hidden
        : ESlateVisibility::Visible;

    if (HpBarWidget)
    {
        HpBarWidget->SetVisibility(VisibilityState);
    }

    if (LevelTextWidget)
    {
        LevelTextWidget->SetVisibility(VisibilityState);
    }
}
