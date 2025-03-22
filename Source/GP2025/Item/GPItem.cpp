#include "Item/GPItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Network/GPNetworkManager.h"
#include "Character/GPCharacterMyplayer.h"
#include "Engine/World.h"
#include "GPItemStruct.h"

// Sets default values
AGPItem::AGPItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetBoxExtent(FVector(50.f, 50.f, 50.f));
	TriggerBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGPItem::OnOverlapBegin);

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
	ItemStaticMesh->SetupAttachment(RootComponent);
	ItemStaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ItemStaticMesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMesh"));
	ItemSkeletalMesh->SetupAttachment(RootComponent);
	ItemSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ItemSkeletalMesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	// 회전하는 움직임 추가
	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.f, 180.f, 0.f); // 초당 180도 회전
}

// Called when the game starts or when spawned
void AGPItem::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorld()->GetTimerManager().SetTimer(
		OverlapEnableTimerHandle,
		this,
		&AGPItem::EnableOverlap,
		OverlapDelay, 
		false 
	);
}

// Called every frame
void AGPItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation();
	NewLocation.Z += FMath::Sin(GetWorld()->TimeSeconds * 2) * 2;  // 위아래로 흔들리는 효과
	SetActorLocation(NewLocation);
}

void AGPItem::SetupItem(int32 NewItemID, uint8 NewItemtype, int32 NewAmount)
{
	ItemID = NewItemID;
	Amount = NewAmount;

	UDataTable* ItemTable = GetItemDataTable();
	if (!ItemTable) return;

	FGPItemStruct* ItemData = ItemTable->FindRow<FGPItemStruct>(*FString::FromInt(NewItemtype), TEXT(""));
	if (!ItemData) return;

	// 아이템 이름 설정 (디버깅용)
	UE_LOG(LogTemp, Log, TEXT("Spawning Item: %s"), *ItemData->ItemName.ToString());

	// Static Mesh가 있는 경우 적용
	if (ItemData->ItemStaticMesh)
	{
		ItemStaticMesh->SetStaticMesh(ItemData->ItemStaticMesh);
		ItemStaticMesh->SetVisibility(true);
		ItemSkeletalMesh->SetVisibility(false);

		UE_LOG(LogTemp, Log, TEXT("Set Static Mesh"));
	}
	else if (ItemData->ItemSkeletalMesh)
	{
		ItemSkeletalMesh->SetSkeletalMesh(ItemData->ItemSkeletalMesh);
		ItemStaticMesh->SetVisibility(false);
		ItemSkeletalMesh->SetVisibility(true);

		UE_LOG(LogTemp, Log, TEXT("Set Skeletal Mesh"));
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

void AGPItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	AGPCharacterMyplayer* Player = Cast<AGPCharacterMyplayer>(OtherActor);
	if (!Player) return;

	UE_LOG(LogTemp, Warning, TEXT("Item Overlap Detected! ItemID: %d | Player: %s"),
		ItemID, *Player->GetName());

	auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (NetworkMgr)
	{
		UE_LOG(LogTemp, Log, TEXT("Sending PlayerTakeItem Packet for ItemID: %d"), ItemID);
		NetworkMgr->SendPlayerTakeItem(ItemID);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Network Manager is NULL - Failed to Send Item Pickup Packet."));
	}
}

void AGPItem::EnableOverlap()
{
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	UE_LOG(LogTemp, Warning, TEXT("Overlap Enabled for Item ID: %d"), ItemID);
}
