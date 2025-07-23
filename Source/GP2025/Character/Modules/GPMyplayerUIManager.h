// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../GP_Server/Source/Common/Type.h"
#include "GPMyplayerUIManager.generated.h"

enum class EChatFriendNotifyType : uint8
{
	None UMETA(DisplayName = "None"),

	/** ģ�� ��û ���� */
	RequestReceived      UMETA(DisplayName = "ģ�� ��û ����"),       // ���濡�� ����
	RequestSent          UMETA(DisplayName = "ģ�� ��û ����"),       // ���� ����
	RequestRejected      UMETA(DisplayName = "ģ�� ��û ������"),
	RequestCancelled     UMETA(DisplayName = "ģ�� ��û ��ҵ�"),

	/** ģ�� ����/��� */
	Accepted             UMETA(DisplayName = "ģ�� ������"),
	AlreadyFriend        UMETA(DisplayName = "�̹� ģ����"),
	AlreadyRequested     UMETA(DisplayName = "�̹� ģ�� ��û��"),
	SelfRequest          UMETA(DisplayName = "�ڱ� �ڽſ��� ��û"),
	NotFound             UMETA(DisplayName = "������ ã�� �� ����"),

	/** ģ�� ���� ���� */
	Removed              UMETA(DisplayName = "ģ�� ������"),

	/** ��Ÿ ���� */
	DBError              UMETA(DisplayName = "DB ����"),
	UnknownError         UMETA(DisplayName = "�� �� ���� ����"),
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
	// Quest Entry ���� �߰��� �Լ�
	void AddQuestEntry(uint8 QuestType, bool bIsSuccess = false);

	// Quest Entry ���� ���ſ� �Լ�
	void UpdateQuestState(uint8 QuestType, bool bIsSuccess);

	// QuestList ��ü ���ٿ� Getter (����� ����� Ȯ�强�� ����)
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


	// ���� NPC �� �����ִ� QuestWidget �� ������ ����
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
	void FocusChatInput();     // �Է�â ���� + ��Ŀ�� + ���콺 ǥ��
	// void CloseChatInput();     // �Է� ���� + ���콺 ���� + �ΰ��� ����


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

