// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GPCharacterNPC.h"
#include "Character/GPCharacterControlData.h"
#include "Components/WidgetComponent.h"
#include "Shop/GPShop.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Components/SphereComponent.h"
#include "Character/Modules/GPMyplayerInputHandler.h"
#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "Quest/GPQuestMessageStruct.h"
#include "UI/GPQuestWidget.h"
#include "Inventory/GPItemSlot.h"
#include "Components/TextBlock.h"
#include "UI/GPNPCInteractionText.h"
#include "UI/GPInGameWidget.h"
#include "Character/Modules/GPMyplayerSoundManager.h"
#include "Kismet/GameplayStatics.h"
#include "Character/Modules/GPMyplayerUIManager.h"
#include "Character/GPCharacterMyplayer.h"
#include "Network/GPNetworkManager.h"

AGPCharacterNPC::AGPCharacterNPC()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ShopBP(TEXT("/Game/Shop/Widgets/WBP_Shop"));
	if (ShopBP.Succeeded())
	{
		ShopWidgetClass = ShopBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> QuestBP(TEXT("/Game/UI/WBP_Quest"));
	if (QuestBP.Succeeded())
	{
		QuestWidgetClass = QuestBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBP(TEXT("/Game/UI/WBP_ShopNPCInteraction.WBP_ShopNPCInteraction_C"));
	if (WidgetBP.Succeeded())
	{
		WBPClass_NPCInteraction = WidgetBP.Class;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> QuestMessageTableObj(
		TEXT("/Game/Quest/GPQuestMessageTable.GPQuestMessageTable")
	);

	if (QuestMessageTableObj.Succeeded())
	{
		QuestMessageTable = QuestMessageTableObj.Object;
	}

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(250.f);
	InteractionSphere->SetCollisionProfileName(TEXT("Trigger"));
	InteractionSphere->SetGenerateOverlapEvents(true);

	// 설명 UI
	InteractionWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidgetComponent->SetupAttachment(RootComponent);
	InteractionWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidgetComponent->SetDrawSize(FVector2D(150.f, 50.f));
	InteractionWidgetComponent->SetVisibility(false);
}

void AGPCharacterNPC::BeginPlay()
{
	Super::BeginPlay();
	if (InteractionSphere)
	{
		InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AGPCharacterNPC::OnInteractionStart);
		InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AGPCharacterNPC::OnInteractionExit);
	}

	if (WBPClass_NPCInteraction && InteractionWidgetComponent)
	{
		InteractionWidgetComponent->SetWidgetClass(WBPClass_NPCInteraction);
	}

	if (InteractionWidgetComponent)
	{
		if (UGPNPCInteractionText* InteractionTextWidget = Cast<UGPNPCInteractionText>(InteractionWidgetComponent->GetUserWidgetObject()))
		{
			FText DescText;

			switch (NPCType)
			{
			case ENPCType::GSSHOP:
			case ENPCType::SUITSHOP:
			case ENPCType::JUICESHOP:
				DescText = FText::FromString(TEXT("상점 열기"));
				break;
			case ENPCType::PROFESSOR:
			case ENPCType::STUDENT:
			case ENPCType::SECURITY:
				DescText = FText::FromString(TEXT("대화하기"));
				break;
			default:
				break;
			}

			if (InteractionTextWidget->NPCText)
			{
				InteractionTextWidget->NPCText->SetText(DescText);
			}
		}
	}
}

void AGPCharacterNPC::OpenShopUI(APlayerController* PlayerController)
{
	if (!ShopWidget && ShopWidgetClass)
	{
		ShopWidget = CreateWidget<UUserWidget>(PlayerController, ShopWidgetClass);
		if (ShopWidget)
		{
			AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(PlayerController->GetPawn());

			if (MyPlayer->UIManager->bIsInventoryOpen) return;

			if (UGPShop* LocalShopWidget = Cast<UGPShop>(ShopWidget))
			{
				LocalShopWidget->SetOwningNPC(this);  // Owning NPC 설정

				if (MyPlayer)
				{
					LocalShopWidget->SetMyPlayer(MyPlayer);
				}

				EShopType ShopType = static_cast<EShopType>(NPCType);
				LocalShopWidget->SetShopType(ShopType);

				if (!LocalShopWidget->ItemDataTable)
				{
					FString DataTablePath = "/Game/Item/GPItemTable.GPItemTable";
					LocalShopWidget->ItemDataTable = Cast<UDataTable>(
						StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath)
					);

					if (!LocalShopWidget->ItemDataTable)
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed to load DataTable: %s"), *DataTablePath);
					}
					else
					{
						UE_LOG(LogTemp, Log, TEXT("ItemDataTable successfully loaded: %s"), *DataTablePath);
					}
				}

				if (!LocalShopWidget->SlotClass)
				{
					static const FString SlotClassPath = "/Game/Shop/Widgets/WBP_SlotForShop.WBP_SlotForShop_C";
					UClass* LoadedSlotClass = Cast<UClass>(
						StaticLoadObject(UClass::StaticClass(), nullptr, *SlotClassPath)
					);

					if (LoadedSlotClass)
					{
						LocalShopWidget->SlotClass = LoadedSlotClass;
						UE_LOG(LogTemp, Log, TEXT("SlotClass successfully loaded: %s"), *SlotClassPath);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Failed to load SlotClass: %s"), *SlotClassPath);
						LocalShopWidget->SlotClass = UGPItemSlot::StaticClass(); 
					}
				}

				LocalShopWidget->PopulateShopItems();
				LocalShopWidget->PopulateSellItems();

				ShopWidget->AddToViewport();
				LocalShopWidget->OpenShopAnimation();
			}
		}
	}

	if (ShopWidget)
	{
		ShopWidget->SetVisibility(ESlateVisibility::Visible);
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->bShowMouseCursor = true;
	}
}

void AGPCharacterNPC::CloseShopUI()
{
	if (ShopWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CloseShopUI] CloseShopUI called. ShopWidget is valid."));

		UWidgetAnimation* CloseAnim = nullptr;

		UGPShop* ShopUI = Cast<UGPShop>(ShopWidget);

		if (ShopUI)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CloseShopUI] Calling ShopUI->CloseShopAnimation()."));
			ShopUI->CloseShopAnimation();

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([this]()
					{
						UE_LOG(LogTemp, Warning, TEXT("[CloseShopUI] Timer triggered → Removing ShopWidget from Parent."));

						if (ShopWidget)
						{
							ShopWidget->RemoveFromParent();
							ShopWidget = nullptr;
							UE_LOG(LogTemp, Warning, TEXT("[CloseShopUI] ShopWidget removed from parent and nulled."));
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("[CloseShopUI] ShopWidget already NULL when timer fired!"));
						}

						APlayerController* PC = GetWorld()->GetFirstPlayerController();
						if (PC)
						{
							PC->SetInputMode(FInputModeGameOnly());
							PC->bShowMouseCursor = false;
							PC->FlushPressedKeys();
							UE_LOG(LogTemp, Warning, TEXT("[CloseShopUI] SetInputMode(GameOnly) and Hide Mouse Cursor."));
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("[CloseShopUI] PlayerController is NULL."));
						}

						bIsInteracting = false;
						UE_LOG(LogTemp, Warning, TEXT("[CloseShopUI] bIsInteracting set to false."));

					}),
				0.5f,
				false
			);
		}
	}
}

void AGPCharacterNPC::OpenQuestUI(APlayerController* PlayerController)
{
	if (!QuestWidget && QuestWidgetClass)
	{
		QuestWidget = CreateWidget<UUserWidget>(PlayerController, QuestWidgetClass);
		if (QuestWidget)
		{
			QuestWidget->AddToViewport();
			if (UGPQuestWidget* LocalQuestWidget = Cast<UGPQuestWidget>(QuestWidget))
			{
				LocalQuestWidget->OwningNPC = this;

				uint8 RowID = 40;
				switch (NPCType)
				{
				case ENPCType::PROFESSOR:
					RowID = 41;
					break;
				case ENPCType::STUDENT:
					RowID = 42;
					break;
				case ENPCType::SECURITY:
					RowID = 43;
					break;
				}

				FText QuestTitle = FText::FromString(TEXT("대화"));
				FText QuestDescription = FText::FromString(TEXT("이 NPC는 특별한 대화가 없습니다."));

				if (QuestMessageTable)
				{
					const FName RowName = FName(*FString::FromInt(RowID));
					FGPQuestMessageStruct* Row = QuestMessageTable->FindRow<FGPQuestMessageStruct>(RowName, TEXT("QuestWidgetLookup"));

					if (Row)
					{
						QuestTitle = Row->QuestTitle;
						QuestDescription = Row->QuestMessage;
					}
				}

				LocalQuestWidget->SetQuestTitle(QuestTitle);
				LocalQuestWidget->SetQuestDescription(QuestDescription);


				// MyPlayer 바인딩 유지
				if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0)))
				{
					if (MyPlayer->UIManager)
					{
						MyPlayer->UIManager->CurrentQuestWidget = LocalQuestWidget;
					}
				}
			}
		}
	}

	if (QuestWidget)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
}

void AGPCharacterNPC::CloseQuestUI()
{
	if (QuestWidget)
	{
		QuestWidget->RemoveFromParent();     
		QuestWidget = nullptr;
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
			PC->FlushPressedKeys();
		}
	}

	bIsInteracting = false;
}

void AGPCharacterNPC::ShowQuestNotAvailableMessage(AGPCharacterMyplayer* MyPlayer, const FText& Message)
{
	if (MyPlayer && MyPlayer->UIManager && MyPlayer->UIManager->GetInGameWidget())
	{
		MyPlayer->UIManager->GetInGameWidget()->ShowGameMessage(Message, 3.0f);
	}

	if (MyPlayer && MyPlayer->SoundManager && MyPlayer->SoundManager->WarningSound)
	{
		MyPlayer->SoundManager->PlaySFX(MyPlayer->SoundManager->WarningSound);
	}
}

void AGPCharacterNPC::OnInteractionStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(OtherActor);
	if (!MyPlayer || !MyPlayer->InputHandler) return;

	InteractionWidgetComponent->SetVisibility(true);
	MyPlayer->InputHandler->CurrentInteractionTarget = this;
}

void AGPCharacterNPC::OnInteractionExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(OtherActor);
	if (!MyPlayer) return;
	InteractionWidgetComponent->SetVisibility(false);
	MyPlayer->InputHandler->CurrentInteractionTarget = nullptr;

	if (bIsInteracting)
	{
		switch (NPCType)
		{
		case ENPCType::GSSHOP:
			CloseShopUI();
			break;
		case ENPCType::SUITSHOP:
			CloseShopUI();
			break;
		case ENPCType::JUICESHOP:
			CloseShopUI();
			break;
		case ENPCType::PROFESSOR:
		case ENPCType::SECURITY:
		case ENPCType::STUDENT:
			MyPlayer->CameraHandler->StopDialogueCamera();
			CloseQuestUI();
			break;
		default:
			break;
		}

		bIsInteracting = false;
	}
}

void AGPCharacterNPC::CheckAndHandleInteraction(AGPCharacterMyplayer* MyPlayer)
{
	if (!MyPlayer || !MyPlayer->InputHandler) return;
	APlayerController* PC = Cast<APlayerController>(MyPlayer->GetController());
	if (!PC) return;
	if (bIsInteracting)
	{
		return;
	}

	auto GetQuestNotAvailableMessage = [this]() -> FText
		{
			FText Message = FText::FromString(TEXT("퀘스트를 진행할 수 없습니다."));

			if (QuestMessageTable)
			{
				FGPQuestMessageStruct* Row = QuestMessageTable->FindRow<FGPQuestMessageStruct>(
					FName(TEXT("40")),
					TEXT("QuestNotAvailableLookup")
				);

				if (Row)
				{
					Message = Row->QuestMessage;
				}
			}

			return Message;
		};

	switch (NPCType)
	{
	case ENPCType::GSSHOP:
		OpenShopUI(PC);
		bIsInteracting = true;
		break;
	case ENPCType::SUITSHOP:
		OpenShopUI(PC);
		bIsInteracting = true;
		break;
	case ENPCType::JUICESHOP:
		OpenShopUI(PC);
		bIsInteracting = true;
		break;
	case ENPCType::PROFESSOR:
		if (MyPlayer->CharacterInfo.CurrentQuest.QuestType != QuestType::CH3_RETURN_TO_TIP_WITH_DOC)
		{
			ShowQuestNotAvailableMessage(MyPlayer, GetQuestNotAvailableMessage());
			return;
		}

		MyPlayer->CameraHandler->StartDialogueCamera(GetActorLocation());
		GetWorld()->GetTimerManager().SetTimer(
			QuestOpenUITimerHandle,
			this,
			&AGPCharacterNPC::OpenQuestUIDelayed,
			1.f,
			false
		);
		break;
	case ENPCType::STUDENT:
		if (MyPlayer->CharacterInfo.CurrentQuest.QuestType != QuestType::CH1_TALK_TO_STUDENT_A)
		{
			ShowQuestNotAvailableMessage(MyPlayer, GetQuestNotAvailableMessage());
			return;
		}

		MyPlayer->CameraHandler->StartDialogueCamera(GetActorLocation());
		GetWorld()->GetTimerManager().SetTimer(
			QuestOpenUITimerHandle,
			this,
			&AGPCharacterNPC::OpenQuestUIDelayed,
			1.f,
			false
		);
		break;
	case ENPCType::SECURITY:
		if (MyPlayer->CharacterInfo.CurrentQuest.QuestType != QuestType::CH1_FIND_JANITOR)
		{
			ShowQuestNotAvailableMessage(MyPlayer, GetQuestNotAvailableMessage());
			return;
		}

		MyPlayer->CameraHandler->StartDialogueCamera(GetActorLocation());
		GetWorld()->GetTimerManager().SetTimer(
			QuestOpenUITimerHandle,
			this,
			&AGPCharacterNPC::OpenQuestUIDelayed,
			1.f,
			false
		);
		break;

	default:
		break;
	}
}

void AGPCharacterNPC::ExitInteraction()
{
	if (!bIsInteracting) return;

	switch (NPCType)
	{
	case ENPCType::GSSHOP:
	case ENPCType::SUITSHOP:
	case ENPCType::JUICESHOP:
		CloseShopUI();
		break;
		 
	case ENPCType::PROFESSOR:
	case ENPCType::STUDENT:
	case ENPCType::SECURITY:
		if (AGPCharacterMyplayer* MyPlayer = Cast<AGPCharacterMyplayer>(UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			if (MyPlayer->CameraHandler)
			{
				MyPlayer->CameraHandler->StopDialogueCamera();
			}

			MyPlayer->UIManager->CurrentQuestWidget = nullptr;
		}
		CloseQuestUI();
		break;
	default:
		break;
	}

	// bIsInteracting = false;
}

void AGPCharacterNPC::OpenQuestUIDelayed()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		OpenQuestUI(PC);
		bIsInteracting = true;
	}
}
