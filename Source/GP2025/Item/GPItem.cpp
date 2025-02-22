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

	// ���� �ν��Ͻ��� ���� ������ ���̺��� ��������
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance) return;

	UDataTable* ItemTable = GetItemDataTable();
	if (!ItemTable) return;

	FGPItemStruct* ItemData = ItemTable->FindRow<FGPItemStruct>(*FString::FromInt(ItemID), TEXT(""));
	if (!ItemData) return;

	// ������ �̸� ���� (������)
	UE_LOG(LogTemp, Log, TEXT("Spawning Item: %s"), *ItemData->ItemName.ToString());

	// Static Mesh�� �ִ� ��� ����
	if (ItemData->ItemStaticMesh)
	{
		ItemStaticMesh->SetStaticMesh(ItemData->ItemStaticMesh);
		ItemStaticMesh->SetVisibility(true);
		ItemSkeletalMesh->SetVisibility(false);
	}
	// Skeletal Mesh�� �ִ� ��� ����
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

	// ���� �ν��Ͻ� ��������
	UGPGameInstance* GameInstance = Cast<UGPGameInstance>(GetGameInstance());
	if (!GameInstance) return;

	// ������ ID�� 400 �̻��̸� ��� (Gold)
	if (ItemID >= 400 && ItemID < 500)
	{
		UE_LOG(LogTemp, Log, TEXT("Player picked up %d gold!"), Amount);
	}
	// �Ϲ� �������̸� �κ��丮�� �߰�
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Player picked up item ID: %d"), ItemID);
	}

	Destroy();
}

