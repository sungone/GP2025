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
	//UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	//class UScrollBox* QuestListScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	TSubclassOf<class UGPQuestListEntryWidget> QuestListEntryClass;

private:
	UPROPERTY()
	TMap<uint8, class UGPQuestListEntryWidget*> QuestEntries;
};