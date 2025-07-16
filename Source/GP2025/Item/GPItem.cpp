#include "Item/GPItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Network/GPNetworkManager.h"
#include "Character/GPCharacterMyplayer.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "GPItemStruct.h"
#include "Components/WidgetComponent.h"
#include "NiagaraComponent.h"
#include "Character/Modules/GPMyplayerInputHandler.h"
#include "Character/Modules/GPMyplayerSoundManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "GameFramework/PlayerController.h"
#include "ObjectPool/GPItemPool.h"

// Sets default values
AGPItem::AGPItem()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
	ItemStaticMesh->SetupAttachment(RootComponent);
	ItemStaticMesh->SetCollisionProfileName(TEXT("NoCollision"));

	//ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMesh"));
	//ItemSkeletalMesh->SetupAttachment(RootComponent);
	//ItemSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));

	ItemInteractionWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInteractionWidgetComp"));
	ItemInteractionWidgetComp->SetupAttachment(RootComponent);
	ItemInteractionWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	ItemInteractionWidgetComp->SetDrawSize(FVector2D(150.f, 50.f));
	ItemInteractionWidgetComp->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraEffectFinder(TEXT("/Game/effect/ARPGEssentials/Effects/NS_ARPGEssentials_Buff_Loop_02.NS_ARPGEssentials_Buff_Loop_02"));
	if (NiagaraEffectFinder.Succeeded())
	{
		SpawnEffect = NiagaraEffectFinder.Object;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Inventory/Widgets/WBP_ItemInteraction"));
	if (WidgetClassFinder.Succeeded())
	{
		ItemInteractionWidgetClass = WidgetClassFinder.Class;
		ItemInteractionWidgetComp->SetWidgetClass(ItemInteractionWidgetClass);
	}

	OverlappingPlayer = nullptr;
}

void AGPItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFloatingEffect();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	AGPCharacterMyplayer* Player = Cast<AGPCharacterMyplayer>(PlayerPawn);
	if (!Player) return;

	const float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	const float InteractRange = 300.f;

	if (Distance < InteractRange)
	{
		OverlappingPlayer = Player;
		ShowInteractionWidget();

		if (OverlappingPlayer->InputHandler && OverlappingPlayer->InputHandler->bGetTakeItem)
		{
			TryTakeItem();
		}
	}
	else
	{
		OverlappingPlayer = nullptr;
		HideInteractionWidget();
	}
}

void AGPItem::UpdateFloatingEffect()
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Z += FMath::Sin(GetWorld()->TimeSeconds * 2.f) * 1.2f;
	SetActorLocation(NewLocation);
}

void AGPItem::ShowInteractionWidget()
{
	if (!ItemInteractionWidgetComp->IsVisible())
	{
		ItemInteractionWidgetComp->SetVisibility(true);
	}

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (!PC) return;

		FVector CamLoc;
		FRotator CamRot;
		PC->GetPlayerViewPoint(CamLoc, CamRot);

		const FVector Right = CamRot.Quaternion().GetRightVector();
		const FVector WidgetOffset = Right * 80.f + FVector(0, 0, 50);

		FVector NewWidgetLocation = GetActorLocation() + WidgetOffset;
		ItemInteractionWidgetComp->SetWorldLocation(NewWidgetLocation);
		ItemInteractionWidgetComp->SetWorldRotation(CamRot);
}

void AGPItem::HideInteractionWidget()
{
	if (ItemInteractionWidgetComp->IsVisible())
		ItemInteractionWidgetComp->SetVisibility(false);
}

void AGPItem::TryTakeItem()
{
	if (!OverlappingPlayer) return;
	if (!OverlappingPlayer->InputHandler || !OverlappingPlayer->InputHandler->bGetTakeItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Item] TakeItem attempted without valid TakeInteraction flag."));
		return;
	}

	auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (NetworkMgr)
	{
		NetworkMgr->SendMyTakeItem(ItemID);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SendPlayerTakeItem!"));
	}

	if (OverlappingPlayer && OverlappingPlayer->SoundManager && OverlappingPlayer->SoundManager->PickUpItemSound)
	{
		OverlappingPlayer->SoundManager->PlaySFX(OverlappingPlayer->SoundManager->PickUpItemSound);
	}

	if (SpawnedEffectComp)
	{
		SpawnedEffectComp->Deactivate();
		SpawnedEffectComp = nullptr;
	}

	OverlappingPlayer = nullptr;
	HideInteractionWidget();
}

void AGPItem::SetupItem(int32 NewItemID, uint8 NewItemtype, int32 NewAmount)
{
	ItemID = NewItemID;
	Amount = NewAmount;

	UDataTable* ItemTable = GetItemDataTable();
	if (!ItemTable) return;

	FGPItemStruct* ItemData = ItemTable->FindRow<FGPItemStruct>(*FString::FromInt(NewItemtype), TEXT(""));
	if (!ItemData) return;

	UE_LOG(LogTemp, Log, TEXT("Spawning Item: %s"), *ItemData->ItemName.ToString());

	if (ItemData->ItemStaticMesh)
	{
		ItemStaticMesh->SetStaticMesh(ItemData->ItemStaticMesh);
		ItemStaticMesh->SetVisibility(true);
	}

	if (SpawnEffect)
	{
		if (SpawnedEffectComp)
		{
			SpawnedEffectComp->DestroyComponent();
			SpawnedEffectComp = nullptr;
		}

		SpawnedEffectComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			SpawnEffect,
			RootComponent,
			NAME_None,
			FVector(0.f, 0.f, 10.f), 
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
}

UDataTable* AGPItem::GetItemDataTable()
{
	static const FString DataTablePath = TEXT("/Game/Item/GPItemTable.GPItemTable");
	UDataTable* DataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath));

	if (!DataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Item DataTable at path: %s"), *DataTablePath);
		return nullptr;
	}

	return DataTable;
}

void AGPItem::Reset()
{
	Super::Reset();
	SetActorLocation(FVector::ZeroVector);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (ItemStaticMesh)
	{
		ItemStaticMesh->SetStaticMesh(nullptr);
		ItemStaticMesh->SetVisibility(false);
	}

	HideInteractionWidget();

	ItemID = -1;
	Amount = 0;
}

void AGPItem::ReturnToPool()
{
	if (Pool)
	{
		Pool->Release(this);
	}
	else
	{
		Destroy();
	}
}

void AGPItem::SetPool(UGPItemPool* InPool)
{
	Pool = InPool;
}