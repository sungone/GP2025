// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPFloatingQuestText.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AGPFloatingQuestText::AGPFloatingQuestText()
{
	PrimaryActorTick.bCanEverTick = true;

	QuestWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("QuestWidget"));
	QuestWidgetComponent->SetupAttachment(RootComponent);
	QuestWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	QuestWidgetComponent->SetDrawSize(FVector2D(500.f, 150.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/UI/WBP_QuestMessageText.WBP_QuestMessageText_C"));
	if (WidgetClass.Succeeded())
		QuestWidgetComponent->SetWidgetClass(WidgetClass.Class);
}

void AGPFloatingQuestText::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* Widget = Cast<UUserWidget>(QuestWidgetComponent->GetWidget());
	if (Widget)
	{
		QuestMessageText = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("QuestMessageText")));
	}

	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &AGPFloatingQuestText::DestroySelf, 2.0f, false);
}

void AGPFloatingQuestText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation();
	NewLocation.Z += DeltaTime * 30.f;
	SetActorLocation(NewLocation);
}

void AGPFloatingQuestText::SetQuestMessage(const FString& Message)
{
	if (QuestMessageText)
	{
		QuestMessageText->SetText(FText::FromString(Message));
	}
}

void AGPFloatingQuestText::DestroySelf()
{
	Destroy();
}