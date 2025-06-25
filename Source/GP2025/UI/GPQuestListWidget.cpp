#include "UI/GPQuestListWidget.h"
#include "UI/GPQuestListEntryWidget.h"
#include "Components/ScrollBox.h"

void UGPQuestListWidget::NativeConstruct()
{
    Super::NativeConstruct();
	QuestEntries.Empty();
	QuestEntryQueue.Empty();

	List0->SetVisibility(ESlateVisibility::Hidden);
	List1->SetVisibility(ESlateVisibility::Hidden);
	List2->SetVisibility(ESlateVisibility::Hidden);
	List3->SetVisibility(ESlateVisibility::Hidden);

	if (!QuestListEntryClass)
	{
		QuestListEntryClass = LoadClass<UGPQuestListEntryWidget>(nullptr, TEXT("/Game/UI/WBP_QuestListEntry.WBP_QuestListEntry_C"));

		if (!QuestListEntryClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load QuestListEntryClass!"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("QuestListEntryClass loaded successfully in NativeConstruct."));
		}
	}
}

void UGPQuestListWidget::AddQuestEntry(uint8 QuestType, bool bIsSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("=== [QuestListWidget] AddQuestEntry called: QuestType = %d ==="), QuestType);

	if (!QuestListEntryClass || QuestEntries.Contains(QuestType))
		return;

	// ���� ����Ʈ �Ϸ� ó��
	if (QuestEntryQueue.Num() > 0)
	{
		if (UGPQuestListEntryWidget* LastQuest = QuestEntryQueue.Last())
		{
			LastQuest->SetQuestState(true);
		}
	}

	UGPQuestListEntryWidget* NewEntry = CreateWidget<UGPQuestListEntryWidget>(GetWorld(), QuestListEntryClass);
	if (!NewEntry)
	{
		UE_LOG(LogTemp, Error, TEXT("[QuestListWidget] Failed to create QuestListEntryWidget"));
		return;
	}

	NewEntry->EntryType = QuestType;
	NewEntry->SetQuestTask(QuestType);
	NewEntry->SetQuestState(bIsSuccess);
	QuestEntries.Add(QuestType, NewEntry);

	// �ִ� 3�������� ���� (ť ����)
	if (QuestEntryQueue.Num() >= 3)
	{
		QuestEntryQueue.RemoveAt(0);
		PlayQuestListAnimation(); // ��ȯ �ִϸ��̼�
	}

	QuestEntryQueue.Add(NewEntry);
	RefreshQuestSlotWidgets();
}

void UGPQuestListWidget::UpdateQuestState(uint8 QuestType, bool bIsSuccess)
{
    if (UGPQuestListEntryWidget** FoundEntry = QuestEntries.Find(QuestType))
    {
        (*FoundEntry)->SetQuestState(bIsSuccess);
    }
}

void UGPQuestListWidget::RefreshQuestSlotWidgets()
{
	// �ʱ�ȭ: �� ����� ����
	List0->SetVisibility(ESlateVisibility::Hidden);
	List1->SetVisibility(ESlateVisibility::Hidden);
	List2->SetVisibility(ESlateVisibility::Hidden);
	List3->SetVisibility(ESlateVisibility::Hidden); // ������ ���� ó��

	for (int32 i = 0; i < QuestEntryQueue.Num(); ++i)
	{
		UGPQuestListEntryWidget* Data = QuestEntryQueue[i];
		if (!Data) continue;

		switch (i)
		{
		case 0: List0->CopyFrom(Data); List0->SetVisibility(ESlateVisibility::Visible); break;
		case 1: List1->CopyFrom(Data); List1->SetVisibility(ESlateVisibility::Visible); break;
		case 2: List2->CopyFrom(Data); List2->SetVisibility(ESlateVisibility::Visible); break;
		}
	}
}

