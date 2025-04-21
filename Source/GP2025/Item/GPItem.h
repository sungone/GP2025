#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GPItem.generated.h"

UCLASS()
class GP2025_API AGPItem : public AActor
{
	GENERATED_BODY()

public:
	AGPItem();

public:
	virtual void Tick(float DeltaTime) override;

	void SetupItem(int32 NewItemID, uint8 NewItemtype, int32 NewAmount);

private:
	void UpdateFloatingEffect();
	void ShowInteractionWidget();
	void HideInteractionWidget();
	void TryTakeItem();
	class UDataTable* GetItemDataTable();
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	class UStaticMeshComponent* ItemStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	class USkeletalMeshComponent* ItemSkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	class UWidgetComponent* ItemInteractionWidgetComp;

	UPROPERTY()
	TSubclassOf<class UUserWidget> ItemInteractionWidgetClass;

	UPROPERTY()
	class AGPCharacterMyplayer* OverlappingPlayer;

	UPROPERTY()
	int32 ItemID;

	UPROPERTY()
	int32 Amount;
};