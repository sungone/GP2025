// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPFloatingDamageText.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGPFloatingDamageText::AGPFloatingDamageText()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageWidget"));
	DamageWidgetComponent->SetupAttachment(RootComponent);
	DamageWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DamageWidgetComponent->SetDrawSize(FVector2D(200.f, 100.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/UI/WBP_DamageText.WBP_DamageText_C"));
	if (WidgetClass.Succeeded())
		DamageWidgetComponent->SetWidgetClass(WidgetClass.Class);
}

// Called when the game starts or when spawned
void AGPFloatingDamageText::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* Widget = Cast<UUserWidget>(DamageWidgetComponent->GetWidget());
	if (Widget)
	{
		DamageText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("DamageText")));
		DamageText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	}

	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AGPFloatingDamageText::DestroySelf, 1.0f, false);
}

// Called every frame
void AGPFloatingDamageText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation();
	NewLocation.Z += DeltaTime * 50.f;
	SetActorLocation(NewLocation);
}

void AGPFloatingDamageText::SetDamageText(float DamageAmount, bool bIsCrt)
{
	if (DamageText)
	{
		if (DamageAmount <= 0.f)
		{
			DamageText->SetText(FText::FromString(TEXT("Miss")));
		}
		else
		{
			DamageText->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), DamageAmount)));

			if (bIsCrt)
			{
				DamageText->SetRenderScale(FVector2D(1.5f, 1.5f)); // 크리티컬일 때 크기 증가
			}
			else
			{
				DamageText->SetRenderScale(FVector2D(0.5f, 0.5f)); // 기본 크기 유지
			}
		}
	}
}

void AGPFloatingDamageText::DestroySelf()
{
	Destroy();
}

