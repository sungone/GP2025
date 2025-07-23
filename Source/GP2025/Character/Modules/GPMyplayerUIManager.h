// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../GP_Server/Source/Common/Type.h"
#include "GPMyplayerUIManager.generated.h"

enum class EChatFriendNotifyType : uint8
{
	None UMETA(DisplayName = "None"),

	/** 친구 신청 관련 */
	RequestReceived      UMETA(DisplayName = "친구 신청 받음"),       // 상대방에게 받음
	RequestSent          UMETA(DisplayName = "친구 신청 보냄"),       // 내가 보냄
	RequestRejected      UMETA(DisplayName = "친구 신청 거절됨"),
	RequestCancelled     UMETA(DisplayName = "친구 신청 취소됨"),

	/** 친구 수락/등록 */
	Accepted             UMETA(DisplayName = "친구 수락됨"),
	AlreadyFriend        UMETA(DisplayName = "이미 친구임"),
	AlreadyRequested     UMETA(DisplayName = "이미 친구 요청함"),
	SelfRequest          UMETA(DisplayName = "자기 자신에게 요청"),
	NotFound             UMETA(DisplayName = "유저를 찾을 수 없음"),

	/** 친구 삭제 관련 */
	Removed              UMETA(DisplayName = "친구 삭제됨"),

	/** 기타 오류 */
	DBError              UMETA(DisplayName = "DB 오류"),
	UnknownError         UMETA(DisplayName = "알 수 없는 오류"),
};


/**
 *
 */
UCLASS()
class GP2025_API UGPMyplayerUIManager : public UObject
{
	GENERATED_BODY()

public:
	UGPMyplayerUIManager();
public:
	void Initialize(class AGPCharacterMyplayer* InOwner);

	// Login
	void OnSetUpInGameWidgets();

	// Inventory
	void ToggleInventory();

	void ResetToggleInventory();

	void OpenInventory();

	void CloseInventory();

	UPROPERTY()
	bool bIsInventoryOpen = false;

	// Setting
	void OpenSettingWidget();
	void CloseSettingWidget();          
	void ToggleSettingWidget();
	bool IsSettingWidgetOpen() const;

	// InGame
	void ShowInGameUI();

	// Dead
	void ShowDeadScreen();
	UFUNCTION()
	void OnDeadRespawnComplete();

	// Quest
	// Quest Entry 직접 추가용 함수
	void AddQuestEntry(uint8 QuestType, bool bIsSuccess = false);

	// Quest Entry 상태 갱신용 함수
	void UpdateQuestState(uint8 QuestType, bool bIsSuccess);

	// QuestList 전체 접근용 Getter (참고로 만들면 확장성도 좋음)
	class UGPQuestListWidget* GetQuestListWidget();

public:
	UPROPERTY()
	AGPCharacterMyplayer* Owner;

	UPROPERTY()
	UUserWidget* InventoryWidget;

	UPROPERTY()
	UUserWidget* SettingWidget;

	UPROPERTY()
	UUserWidget* InGameWidget;

	UPROPERTY()
	UUserWidget* GunCrosshairWidget;

	UPROPERTY()
	UUserWidget* LobbyWidget;

	UPROPERTY()
	UUserWidget* LoginWidget;

	UPROPERTY()
	UUserWidget* DeadScreenWidget;


	// 현재 NPC 가 열고있는 QuestWidget 이 있으면 저장
	UPROPERTY()
	class UGPQuestWidget* CurrentQuestWidget = nullptr;

	UPROPERTY()
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> SettingWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> InGameWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> LoginWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> GunCrosshairWidgetClass;


	UPROPERTY()
	TSubclassOf<UUserWidget> DeadScreenWidgetClass;

	bool bInventoryToggled = false;

public:
	class UGPLobbyWidget* GetLobbyWidget();
	class UGPInGameWidget* GetInGameWidget();
	class UGPInventory* GetInventoryWidget();
	class UGPChatBoxWidget* GetChatBoxWidget();

public:
	void ShowLobbyUI();
	void ShowLoginUI();

	// Skill Level UP Text
public:
	void SpawnSkillLevelText(int32 NewLevel);

	// Chat System
	void FocusChatInput();     // 입력창 열기 + 포커스 + 마우스 표시
	// void CloseChatInput();     // 입력 종료 + 마우스 숨김 + 인게임 복귀


// Skill Info Update From Inventory
	void UpdateSkillInfosFromPlayer();


	// Quest Message
	void ShowQuestStartMessage(QuestType InQuestType);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	UDataTable* QuestMessageTable;

	// Main Quest Start Widget
	void PlayTutorialQuestWidget();
	UPROPERTY()
	UUserWidget* MainQuestStartWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	TSubclassOf<UUserWidget> MainQuestStartWidgetClass;

	// Friend
public:
	// FriendBox
	void OpenFriendBox();
	void CloseFriendBox();

	UPROPERTY()
	class UGPFriendBox* FriendBoxWidget;

	UPROPERTY()
	TSubclassOf<UUserWidget> FriendBoxWidgetClass;

	class UGPFriendBox* GetFriendBoxWidget();

// Quest
	void ShowTutorialQuestWidget();

// Skill Text
	void ShowSkillUnlockMessage(ESkillGroup SkillGroup);
	void ShowSkillUpgradeMessage(ESkillGroup SkillGroup);

	void AddFriendSystemMessage(EChatFriendNotifyType NotifyType, const FString& NickName);
	void AddFriendSystemMessage(EChatFriendNotifyType NotifyType);
	void AddFriendSystemMessage(int32 Result);

};

