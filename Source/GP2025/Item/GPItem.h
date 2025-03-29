

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPItem.generated.h"

UCLASS()
class GP2025_API AGPItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGPItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UStaticMeshComponent* ItemStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	USkeletalMeshComponent* ItemSkeletalMesh;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Amount;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class URotatingMovementComponent* RotatingMovement;

	void SetupItem(int32 NewItemID, uint8 NewItemtype, int32 NewMoneyAmount);
	UDataTable* GetItemDataTable();

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// 플레이어가 아이템 먹는걸 딜레이 하기위한 멤버들
public :
	void EnableOverlap();
	FTimerHandle OverlapEnableTimerHandle;
	UPROPERTY(EditAnywhere , Category = "Item")
	float OverlapDelay = 1.5f;

	// Pick Up Guid 
public :
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> ItemInteractionWidgetClass;

	UUserWidget* ItemInteractionWidget;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* ItemInteractionWidgetComp;
};
