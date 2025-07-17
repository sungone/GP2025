// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../GP_Server/Source/Common/Type.h"
#include "GPMyplayerUIManager.generated.h"


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

	// InGame
	void ShowInGameUI();

	// Dead
	void ShowDeadScreen();

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
	void PlayMainQuestStartWidget();
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
};

