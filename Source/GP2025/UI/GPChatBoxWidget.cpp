// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GPChatBoxWidget.h"
#include "Network/GPNetworkManager.h"
#include "Network/GObjectManager.h"
#include "Kismet/GameplayStatics.h"
#include "UI/GPChatMessageWidget.h"
#include "Components/EditableText.h"

void UGPChatBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!ChatMessageWidgetClass)
	{
		TSubclassOf<UGPChatMessageWidget> FoundClass = LoadClass<UGPChatMessageWidget>(
			nullptr, TEXT("/Game/UI/WBP_ChatMessage.WBP_ChatMessage_C"));
		if (FoundClass)
		{
			ChatMessageWidgetClass = FoundClass;
		}
	}

	if (SendMessageText)
	{
		SendMessageText->OnTextCommitted.AddDynamic(this, &UGPChatBoxWidget::OnChatCommitted);
	}

	if (EnterButton)
	{
		EnterButton->OnClicked.AddDynamic(this, &UGPChatBoxWidget::OnEnterButtonClicked);
	}


	if (UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		NetMgr->OnReceiveChat.AddDynamic(this, &UGPChatBoxWidget::HandleChatReceived);
	}
}

void UGPChatBoxWidget::OnChatCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter && !Text.IsEmpty())
	{
		HandleSendMessage();
	}
}

void UGPChatBoxWidget::OnEnterButtonClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}

	HandleSendMessage();
}

void UGPChatBoxWidget::HandleSendMessage()
{
	const FText& Text = SendMessageText->GetText();
	if (Text.IsEmpty())
		return;

	if (UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		EChatChannel Channel = EChatChannel::Zone;
		FString RawText = Text.ToString().TrimStartAndEnd();

		if (RawText.StartsWith("/"))
		{
			if (RawText.Len() >= 2)
			{
				TCHAR CommandChar = RawText[1];

				if (CommandChar == 'w')
				{
					FString Params = RawText.Mid(2).TrimStartAndEnd();
					int32 SpaceIndex;
					if (Params.FindChar(' ', SpaceIndex))
					{
						FString TargetName = Params.Left(SpaceIndex);
						FString Message = Params.Mid(SpaceIndex + 1);

						if (!TargetName.IsEmpty() && !Message.IsEmpty())
						{
							if (UGPObjectManager* ObjMgr = GetWorld()->GetSubsystem<UGPObjectManager>())
							{
								uint32 TargetDBId = ObjMgr->GetFriendDBId(TargetName);
								if (TargetDBId != 0)
								{
									NetMgr->SendMyWhisperMessage(TargetDBId, Message);
								}
								else
								{
									UE_LOG(LogTemp, Warning, TEXT("No such friend: %s"), *TargetName);
								}
							}
						}
					}

					SendMessageText->SetText(FText::GetEmpty());
					if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
					{
						PC->SetShowMouseCursor(false);
						PC->SetInputMode(FInputModeGameOnly());
					}
					return;
				}

				switch (CommandChar)
				{
				case 'a': Channel = EChatChannel::All; break;
				case 'f': Channel = EChatChannel::Friend; break;
				default: Channel = EChatChannel::Zone; break;
				}

				RawText = RawText.Mid(2).TrimStartAndEnd();
			}
		}

		if (!RawText.IsEmpty())
		{
			NetMgr->SendMyChatMessage(RawText, Channel);
		}
	}

	SendMessageText->SetText(FText::GetEmpty());

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		PC->SetShowMouseCursor(false);
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void UGPChatBoxWidget::SetKeyboardFocusToInput()
{
	if (SendMessageText)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateLambda([this]()
				{
					if (SendMessageText)
					{
						SendMessageText->SetKeyboardFocus();
					}
				}),
			0.01f, // 1프레임 지연 (10ms 정도)
			false
		);
	}
}

void UGPChatBoxWidget::AddChatMessage(EChatChannel Channel, const FString& TargetName, const FString& Message)
{
	if (!ChatMessageWidgetClass || !ScrollBox)
	{
		return;
	}
	UGPChatMessageWidget* NewMessageWidget = CreateWidget<UGPChatMessageWidget>(this, ChatMessageWidgetClass);
	if (NewMessageWidget)
	{
		NewMessageWidget->SetChatMessage(Channel, TargetName, Message);
		ScrollBox->AddChild(NewMessageWidget);
		ScrollBox->ScrollToEnd();
	}
}

void UGPChatBoxWidget::HandleChatReceived(uint8 Channel, const FString& Sender, const FString& Message)
{
	EChatChannel ChatChannel = static_cast<EChatChannel>(Channel);
	AddChatMessage(ChatChannel, Sender, Message);
}