// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterNPC.h"
#include "Character/GPCharacterControlData.h"
#include "Shop/GPShop.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Components/SphereComponent.h"
#include "Character/GPCharacterMyplayer.h"

AGPCharacterNPC::AGPCharacterNPC()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ShopBP(TEXT("/Game/Shop/Widgets/WBP_Shop"));
	if (ShopBP.Succeeded())
	{
		ShopWidgetClass = ShopBP.Class;
	}

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(200.f);
	InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));
	InteractionSphere->SetGenerateOverlapEvents(true);
}

void AGPCharacterNPC::BeginPlay()
{
	Super::BeginPlay();
	if (InteractionSphere)
	{
		InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGPCharacterNPC::OnPlayerEnter);
		InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AGPCharacterNPC::OnPlayerExit);
	}
}

void AGPCharacterNPC::OpenShopUI(APlayerController* PlayerController)
{
	if (!ShopWidget && ShopWidgetClass)
	{
		ShopWidget = CreateWidget<UUserWidget>(PlayerController, ShopWidgetClass);
		if (ShopWidget)
		{
			ShopWidget->AddToViewport();
		}
	}

	if (ShopWidget)
	{
		ShopWidget->SetVisibility(ESlateVisibility::Visible);
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->bShowMouseCursor = true;
	}
}

void AGPCharacterNPC::CloseShopUI()
{
	if (ShopWidget)
	{
		ShopWidget->SetVisibility(ESlateVisibility::Hidden);

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}
}

void AGPCharacterNPC::OnPlayerEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if (Cast<AGPCharacterMyplayer>(OtherActor))
	{
		OpenShopUI(PC);
	}
}

void AGPCharacterNPC::OnPlayerExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AGPCharacterMyplayer>(OtherActor))
	{
		CloseShopUI();
	}
}
