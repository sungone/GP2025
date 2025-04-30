// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GPSkillLevelUpText.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AGPSkillLevelUpText::AGPSkillLevelUpText()
{
	PrimaryActorTick.bCanEverTick = true;

	SkillTextWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SkillLevelUpTextWidget"));
	SkillTextWidgetComponent->SetupAttachment(RootComponent);
	SkillTextWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	SkillTextWidgetComponent->SetDrawSize(FVector2D(300.f, 100.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/UI/WBP_SkillLevelUpText.WBP_SkillLevelUpText_C"));
	if (WidgetClass.Succeeded())
	{
		SkillTextWidgetComponent->SetWidgetClass(WidgetClass.Class);
	}
}

void AGPSkillLevelUpText::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* Widget = Cast<UUserWidget>(SkillTextWidgetComponent->GetWidget());
	if (Widget)
	{
		SkillTextBlock = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("SkillLevelUpText")));
	}

	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AGPSkillLevelUpText::DestroySelf, 1.5f, false);
}

void AGPSkillLevelUpText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation();
	NewLocation.Z += DeltaTime * 30.f;
	SetActorLocation(NewLocation);
}

void AGPSkillLevelUpText::SetSkillLevelUpText(int32 SkillLevel)
{
	if (!SkillTextBlock) return;

	FString Message;

	if (SkillLevel == 2)
	{
		Message = FText::FromString(TEXT("Q skill unlocked")).ToString();
	}
	else if (SkillLevel == 3)
	{
		Message = FText::FromString(TEXT("E skill unlocked")).ToString();
	}
	else if (SkillLevel == 4)
	{
		Message = FText::FromString(TEXT("R skill unlocked")).ToString();
	}
	else if (SkillLevel == 5 || SkillLevel == 8)
	{
		Message = FText::FromString(TEXT("Q skill leveled up")).ToString();
	}
	else if (SkillLevel == 6 || SkillLevel == 9)
	{
		Message = FText::FromString(TEXT("E skill leveled up")).ToString();
	}
	else if (SkillLevel == 7 || SkillLevel == 10)
	{
		Message = FText::FromString(TEXT("R skill leveled up")).ToString();
	}
	else
	{
		Message = FText::FromString(TEXT("Skill leveled up!")).ToString();
	}

	SkillTextBlock->SetText(FText::FromString(Message));
}

void AGPSkillLevelUpText::DestroySelf()
{
	Destroy();
}

