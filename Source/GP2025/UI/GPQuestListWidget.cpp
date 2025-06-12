#include "UI/GPQuestListWidget.h"
#include "UI/GPQuestListEntryWidget.h"
#include "Components/ScrollBox.h"

void UGPQuestListWidget::NativeConstruct()
{
    Super::NativeConstruct();
    QuestEntries.Empty();

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

	if (!QuestListEntryClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[QuestListWidget] QuestListEntryClass is NULL"));
		return;
	}

	if (!QuestListScrollBox)
	{
		UE_LOG(LogTemp, Error, TEXT("[QuestListWidget] QuestListScrollBox is NULL"));
		return;
	}

	if (QuestEntries.Contains(QuestType))
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuestListWidget] QuestType %d already exists."), QuestType);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[QuestListWidget] Creating new QuestListEntryWidget"));

	UGPQuestListEntryWidget* NewEntry = CreateWidget<UGPQuestListEntryWidget>(GetWorld(), QuestListEntryClass);
	if (!NewEntry)
	{
		UE_LOG(LogTemp, Error, TEXT("[QuestListWidget] Failed to create QuestListEntryWidget"));
		return;
	}

	NewEntry->EntryType = QuestType;
	NewEntry->SetQuestTask(QuestType);
	NewEntry->SetQuestState(bIsSuccess);

	QuestListScrollBox->AddChild(NewEntry);
	QuestEntries.Add(QuestType, NewEntry);

	UE_LOG(LogTemp, Warning, TEXT("[QuestListWidget] QuestEntry %d added to ScrollBox"), QuestType);
}

void UGPQuestListWidget::UpdateQuestState(uint8 QuestType, bool bIsSuccess)
{
    if (UGPQuestListEntryWidget** FoundEntry = QuestEntries.Find(QuestType))
    {
        (*FoundEntry)->SetQuestState(bIsSuccess);
    }
}