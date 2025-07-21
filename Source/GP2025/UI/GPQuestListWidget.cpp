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
	if (QuestType == static_cast<uint8>(QuestType::TUT_START)
		|| QuestType == static_cast<uint8>(QuestType::TUT_COMPLETE))
	{
		return;
	}

	if (!QuestListEntryClass || QuestEntries.Contains(QuestType))
		return;

	if (QuestEntryQueue.Num() > 0)
	{
		UGPQuestListEntryWidget* LastQuest = QuestEntryQueue.Last();
		if (LastQuest)
			LastQuest->SetQuestState(true);
	}

	UGPQuestListEntryWidget* NewEntry = CreateWidget<UGPQuestListEntryWidget>(GetWorld(), QuestListEntryClass);
	if (!NewEntry)
		return;

	NewEntry->EntryType = QuestType;
	NewEntry->SetQuestTask(QuestType);
	NewEntry->SetQuestState(bIsSuccess);

	QuestEntries.Add(QuestType, NewEntry);

	List2->CopyFrom(List1);
	List2->SetQuestState(true);
	List1->CopyFrom(List0);
	List1->SetQuestState(true);
	List0->CopyFrom(NewEntry);

	List0->SetVisibility(ESlateVisibility::Visible);
	List1->SetVisibility(List1->HasValidData() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	List2->SetVisibility(List2->HasValidData() ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

	PlayQuestListAnimation();

	if (QuestEntryQueue.Num() >= 3)
	{
		QuestEntryQueue.RemoveAt(0);
	}

	QuestEntryQueue.Add(NewEntry);
}

void UGPQuestListWidget::UpdateQuestState(uint8 QuestType, bool bIsSuccess)
{
	if (QuestType == 14) // Tino Quest Clear
	{
		List0->SetQuestState(true);
		return;
	}

	if (UGPQuestListEntryWidget** FoundEntry = QuestEntries.Find(QuestType))
	{
		(*FoundEntry)->SetQuestState(bIsSuccess);
	}
}

