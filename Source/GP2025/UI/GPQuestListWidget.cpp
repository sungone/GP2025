#include "UI/GPQuestListWidget.h"
#include "UI/GPQuestListEntryWidget.h"
#include "Components/ScrollBox.h"

void UGPQuestListWidget::NativeConstruct()
{
    Super::NativeConstruct();
    QuestEntries.Empty();
}

void UGPQuestListWidget::AddQuestEntry(uint8 QuestType, bool bIsSuccess)
{
	if (!QuestListEntryClass || !QuestListScrollBox)
		return;

	if (QuestEntries.Contains(QuestType))
		return;

	UGPQuestListEntryWidget* NewEntry = CreateWidget<UGPQuestListEntryWidget>(GetWorld(), QuestListEntryClass);
	NewEntry->EntryType = QuestType;

	NewEntry->SetQuestTask(QuestType);
	NewEntry->SetQuestState(bIsSuccess);

	QuestListScrollBox->AddChild(NewEntry);
	QuestEntries.Add(QuestType, NewEntry);
}

void UGPQuestListWidget::UpdateQuestState(uint8 QuestType, bool bIsSuccess)
{
    if (UGPQuestListEntryWidget** FoundEntry = QuestEntries.Find(QuestType))
    {
        (*FoundEntry)->SetQuestState(bIsSuccess);
    }
}