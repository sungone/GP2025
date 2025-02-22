// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GPItem.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Network/GPGameInstance.h"
#include "Character/GPCharacterPlayer.h"
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
}

// Called when the game starts or when spawned
void AGPItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGPItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGPItem::SetupItem(int32 NewItemID, int32 NewAmount)
{
	ItemID = NewItemID;
	Amount = NewAmount;

	// 게임 인스턴스를 통해 데이터 테이블을 가져오기
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance) return;

	UDataTable* ItemTable = GetItemDataTable();
	if (!ItemTable) return;

	FGPItemStruct* ItemData = ItemTable->FindRow<FGPItemStruct>(*FString::FromInt(ItemID), TEXT(""));
	if (!ItemData) return;

	// 아이템 이름 설정 (디버깅용)
	UE_LOG(LogTemp, Log, TEXT("Spawning Item: %s"), *ItemData->ItemName.ToString());

	// Static Mesh가 있는 경우 적용
	if (ItemData->ItemStaticMesh)
	{
		ItemStaticMesh->SetStaticMesh(ItemData->ItemStaticMesh);
		ItemStaticMesh->SetVisibility(true);
		ItemSkeletalMesh->SetVisibility(false);
	}
	// Skeletal Mesh가 있는 경우 적용
	else if (ItemData->ItemSkeletalMesh)
	{
		ItemSkeletalMesh->SetSkeletalMesh(ItemData->ItemSkeletalMesh);
		ItemStaticMesh->SetVisibility(false);
		ItemSkeletalMesh->SetVisibility(true);
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

void AGPItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	AGPCharacterPlayer* Player = Cast<AGPCharacterPlayer>(OtherActor);
	if (!Player) return;

	// 게임 인스턴스 가져오기
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance) return;

	// 아이템 ID가 400 이상이면 골드 (Gold)
	if (ItemID >= 400 && ItemID < 500)
	{
		UE_LOG(LogTemp, Log, TEXT("Player picked up %d gold!"), Amount);
	}
	// 일반 아이템이면 인벤토리에 추가
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player picked up item ID: %d"), ItemID);
	}

	Destroy();
}

