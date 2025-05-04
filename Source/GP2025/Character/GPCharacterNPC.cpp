// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterNPC.h"
#include "Character/GPCharacterControlData.h"
#include "Components/WidgetComponent.h"
#include "Shop/GPShop.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Components/SphereComponent.h"
#include "Character/Modules/GPMyplayerInputHandler.h"
#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Character/GPCharacterMyplayer.h"

AGPCharacterNPC::AGPCharacterNPC()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ShopBP(TEXT("/Game/Shop/Widgets/WBP_Shop"));
	if (ShopBP.Succeeded())
	{
		ShopWidgetClass = ShopBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> QuestBP(TEXT("/Game/UI/WBP_Quest"));
	if (QuestBP.Succeeded())
	{
		QuestWidgetClass = QuestBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBP(TEXT("/Game/UI/WBP_ShopNPCInteraction.WBP_ShopNPCInteraction_C"));
	if (WidgetBP.Succeeded())
	{
		WBPClass_NPCInteraction = WidgetBP.Class;
	}

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(400.f);
	InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));
	InteractionSphere->SetGenerateOverlapEvents(true);

	// Ό³Έν UI
	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidgetComponent->SetDrawSize(FVector2D(150.f, 50.f));
	InteractionWidgetComponent->SetVisibility(false);
}

void AGPCharacterNPC::BeginPlay()
{
	Super::BeginPlay();
	if (InteractionSphere)
	{
		InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGPCharacterNPC::OnInteractionToggle);
		InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AGPCharacterNPC::OnInteractionExit);
	}

	if (WBPClass_NPCInteraction && InteractionWidgetComponent)
	{
		InteractionWidgetComponent->SetWidgetClass(WBPClass_NPCInteraction);
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

void AGPCharacterNPC::OpenQuestUI(APlayerController* PlayerController)
{
	if (!QuestWidget && QuestWidgetClass)
	{
		QuestWidget = CreateWidget<UUserWidget>(PlayerController, QuestWidgetClass);
		if (QuestWidget)
		{
			QuestWidget->AddToViewport();
		}
	}

	if (QuestWidget)
	{
		QuestWidget->SetVisibility(ESlateVisibility::Visible);
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->bShowMouseCursor = true;
	}
}

void AGPCharacterNPC::CloseQuestUI()
{
	if (QuestWidget)
	{
		QuestWidget->SetVisibility(ESlateVisibility::Hidden);

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
	}
}

void AGPCharacterNPC::OnInteractionToggle(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(OtherActor);
	if (!MyPlayer || !MyPlayer->InputHandler) return;

	InteractionWidgetComponent->SetVisibility(true);
	MyPlayer->InputHandler->CurrentInteractionTarget = this;
}

void AGPCharacterNPC::OnInteractionExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(OtherActor);
	if (!MyPlayer) return;

	if (bIsInteracting)
	{
		switch (NPCType)
		{
		case ENPCType::SHOP:
			CloseShopUI();
			break;
		case ENPCType::QUEST:
			MyPlayer->CameraHandler->StopDialogueCamera();
			CloseQuestUI();
			break;
		default:
			break;
		}

		bIsInteracting = false;
	}

	MyPlayer->InputHandler->CurrentInteractionTarget = nullptr;
}

void AGPCharacterNPC::CheckAndHandleInteraction(AGPCharacterMyplayer* MyPlayer)
{
	if (!MyPlayer || !MyPlayer->InputHandler) return;
	// if (!MyPlayer->InputHandler->bGetInteraction) return;

	APlayerController* PC = Cast<APlayerController>(MyPlayer->GetController());
	if (!PC) return;

	if (bIsInteracting)
	{
		switch (NPCType)
		{
		case ENPCType::SHOP:
			CloseShopUI();
			break;
		case ENPCType::QUEST:
			MyPlayer->CameraHandler->StopDialogueCamera();
			CloseQuestUI();
			break;
		default:
			break;
		}
		bIsInteracting = false;
	}
	else
	{
		switch (NPCType)
		{
		case ENPCType::SHOP:
			OpenShopUI(PC);
			break;
		case ENPCType::QUEST:
			MyPlayer->CameraHandler->StartDialogueCamera(GetActorLocation());
			GetWorld()->GetTimerManager().SetTimer(
				QuestOpenUITimerHandle,
				this,
				&AGPCharacterNPC::OpenQuestUIDelayed,
				1.f,
				false
			);
			break;
		default:
			break;
		}
		bIsInteracting = true;
	}

	InteractionWidgetComponent->SetVisibility(false);
	MyPlayer->InputHandler->bGetInteraction = false;
}

void AGPCharacterNPC::OpenQuestUIDelayed()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		OpenQuestUI(PC);
	}
}
