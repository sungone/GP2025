// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GPChatBoxWidget.h"
#include "Network/GPNetworkManager.h"
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
	{
		return;
	}

	if (UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		EChatChannel Channel = EChatChannel::All; // 기본값
		FString RawText = Text.ToString().TrimStartAndEnd();

		if (RawText.StartsWith("/"))
		{
			if (RawText.Len() >= 2)
			{
				TCHAR CommandChar = RawText[1];
				switch (CommandChar)
				{
				case 'a':
					Channel = EChatChannel::All;
					break;
				case 'f':
					Channel = EChatChannel::Friend;
					break;
				case 'z':
					Channel = EChatChannel::Zone;
					break;
				default:
					Channel = EChatChannel::All;
					break;
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

void UGPChatBoxWidget::AddChatMessage(uint8 Channel, const FString& UserName, const FString& Message)
{
	if (!ChatMessageWidgetClass || !ScrollBox)
	{
		return;
	}
	UGPChatMessageWidget* NewMessageWidget = CreateWidget<UGPChatMessageWidget>(this, ChatMessageWidgetClass);
	if (NewMessageWidget)
	{
		NewMessageWidget->SetChatMessage(Channel, UserName, Message);
		ScrollBox->AddChild(NewMessageWidget);
		ScrollBox->ScrollToEnd();
	}
}

void UGPChatBoxWidget::HandleChatReceived(uint8 Channel, const FString& Sender, const FString& Message)
{
	AddChatMessage(Channel, Sender, Message);
}