// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPQuestListEntryWidget.h"
#include "Components/TextBlock.h"          
#include "Styling/SlateColor.h"         

void UGPQuestListEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGPQuestListEntryWidget::SetQuestState(bool bIsSuccess)
{
	if (!QuestStateText) return;

	FString StateText = bIsSuccess ? TEXT("성공") : TEXT("진행중");

	QuestStateText->SetText(FText::FromString(StateText));

	FLinearColor StateColor = bIsSuccess ? FLinearColor::Green : FLinearColor::Yellow;
	QuestStateText->SetColorAndOpacity(FSlateColor(StateColor));
}

void UGPQuestListEntryWidget::SetQuestTask(uint8 InQuestType)
{
    UE_LOG(LogTemp, Warning, TEXT("[QuestListEntryWidget] SetQuestTask called: QuestType = %d"), InQuestType);

    if (!QuestTaskText)
    {
        UE_LOG(LogTemp, Error, TEXT("[QuestListEntryWidget] QuestTaskText is NULL! (BindWidget 바인딩 실패 가능성)"));
        return;
    }

    FString TaskDescription;

    switch (static_cast<QuestType>(InQuestType))
    {
        // Chapter 1
    case QuestType::CH1_TALK_TO_STUDENT_A:
        TaskDescription = TEXT("TIP에서 학생 A를 만나기");
        break;
    case QuestType::CH1_GO_TO_E_FIRST:
        TaskDescription = TEXT("E동으로 이동하기");
        break;
    case QuestType::CH1_FIND_JANITOR:
        TaskDescription = TEXT("경비아저씨를 찾기");
        break;
    case QuestType::CH1_GO_TO_BUNKER:
        TaskDescription = TEXT("벙커로 이동하기");
        break;
    case QuestType::CH1_BUNKER_CLEANUP:
        TaskDescription = TEXT("열쇠를 가진 몬스터를 찾기");
        break;
    case QuestType::CH1_FIND_KEY_ITEM:
        TaskDescription = TEXT("열쇠 아이템을 획득하기");
        break;

        // Chapter 2
    case QuestType::CH2_ENTER_E_BUILDING:
        TaskDescription = TEXT("열쇠를 가지고 E동에 입장하기");
        break;
    case QuestType::CH2_CLEAR_E_BUILDING:
        TaskDescription = TEXT("E동의 몬스터 제거하기");
        break;
    case QuestType::CH2_KILL_DESKMON:
        TaskDescription = TEXT("데스크몬 제거하기");
        break;

        // Chapter 3
    case QuestType::CH3_RETURN_TO_TIP_WITH_DOC:
        TaskDescription = TEXT("문서를 가지고 교수를 찾아가기");
        break;
    case QuestType::CH3_CLEAR_SERVER_ROOM:
        TaskDescription = TEXT("산융의 몬스터 제거하기");
        break;
    case QuestType::CH3_KILL_DRILL:
        TaskDescription = TEXT("드릴몬 제거하기");
        break;

        // Chapter 4
    case QuestType::CH4_ENTER_GYM:
        TaskDescription = TEXT("체육관으로 이동하기");
        break;
    case QuestType::CH4_KILL_TINO:
        TaskDescription = TEXT("최종 보스 Tino 제거하기");
        break;

        // Tutorial
    case QuestType::TUT_START:
        TaskDescription = TEXT("튜토리얼 시작");
        break;
    case QuestType::TUT_KILL_ONE_MON:
        TaskDescription = TEXT("몬스터를 한 마리 처치하기");
        break;
    case QuestType::TUT_USE_ITEM:
        TaskDescription = TEXT("회복 아이템을 사용하기");
        break;
    case QuestType::TUT_BUY_ITEM:
        TaskDescription = TEXT("상점에서 무기 아이템을 구매하기");
        break;
    case QuestType::TUT_EQUIP_ITEM:
        TaskDescription = TEXT("구매한 무기 아이템을 착용하기");
        break;
    case QuestType::TUT_COMPLETE:
        TaskDescription = TEXT("튜토리얼 완료!");
        break;

    default:
        TaskDescription = TEXT("알 수 없는 퀘스트입니다.");
        break;
    }


    QuestTaskText->SetText(FText::FromString(TaskDescription));
    UE_LOG(LogTemp, Warning, TEXT("[QuestListEntryWidget] TaskDescription set to: %s"), *TaskDescription);
}

void UGPQuestListEntryWidget::CopyFrom(UGPQuestListEntryWidget* Source)
{
    if (!Source) return;

    this->EntryType = Source->EntryType;

    this->QuestTaskText->SetText(Source->QuestTaskText->GetText());
    this->QuestStateText->SetText(Source->QuestStateText->GetText());

    this->QuestStateText->SetColorAndOpacity(Source->QuestStateText->ColorAndOpacity);
}

bool UGPQuestListEntryWidget::HasValidData() const
{
    return EntryType != 0;
}
