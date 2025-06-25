#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GPQuestListWidget.generated.h"

UCLASS()
class GP2025_API UGPQuestListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	// ����Ʈ ��Ʈ�� �߰�
	void AddQuestEntry(uint8 QuestType, bool bIsSuccess = false);

	// ����Ʈ ���� ����
	void UpdateQuestState(uint8 QuestType, bool bIsSuccess);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TSubclassOf<class UGPQuestListEntryWidget> QuestListEntryClass;

	UPROPERTY()
	TMap<uint8, class UGPQuestListEntryWidget*> QuestEntries;


protected :

	UPROPERTY(meta = (BindWidget))
	UGPQuestListEntryWidget* List0;

	UPROPERTY(meta = (BindWidget))
	UGPQuestListEntryWidget* List1;

	UPROPERTY(meta = (BindWidget))
	UGPQuestListEntryWidget* List2;

	UPROPERTY(meta = (BindWidget))
	UGPQuestListEntryWidget* List3;

	UPROPERTY()
	TArray<UGPQuestListEntryWidget*> QuestEntryQueue;
	void RefreshQuestSlotWidgets();

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void PlayQuestListAnimation();
};