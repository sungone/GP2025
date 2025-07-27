// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GPChatBoxWidget.h"
#include "Network/GPNetworkManager.h"
#include "Network/GPObjectManager.h"
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
	if (CommitMethod != ETextCommit::OnEnter)
		return;

	FString RawText = Text.ToString().TrimStartAndEnd();

	if (RawText.IsEmpty())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly());
		}

		SendMessageText->SetText(FText::GetEmpty());
		return;
	}

	HandleSendMessage();
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
	const FText& InputText = SendMessageText->GetText();
	FString RawText = InputText.ToString().TrimStartAndEnd();

	if (RawText.IsEmpty())
		return;

	UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (!NetMgr)
		return;

	// 기본 채널은 Zone
	EChatChannel Channel = EChatChannel::Zone;

	// 명령어 처리
	if (RawText.StartsWith("/"))
	{
		if (HandleChatCommand(RawText, NetMgr))
		{
			ClearChatInput();
			return;
		}
		else
		{
			ClearChatInput();
			return;
		}
	}

	// 일반 채팅 메시지 전송
	NetMgr->SendMyChatMessage(RawText, Channel);
	ClearChatInput();
}

bool UGPChatBoxWidget::HandleChatCommand(const FString& Input, UGPNetworkManager* NetMgr)
{
	TCHAR CommandChar = Input.Len() > 1 ? Input[1] : 0;

	if (CommandChar == 'w') // Whisper
	{
		FString Params = Input.Mid(2).TrimStartAndEnd();
		int32 SpaceIndex;
		if (Params.FindChar(' ', SpaceIndex))
		{
			FString TargetName = Params.Left(SpaceIndex);
			FString Message = Params.Mid(SpaceIndex + 1);

			if (!TargetName.IsEmpty() && !Message.IsEmpty())
			{
				NetMgr->SendMyWhisperMessage(TargetName, Message);
				return true;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Invalid whisper format. Usage: /w TargetName Message"));
		return false;
	}

	// Other channels: /a, /f
	EChatChannel Channel = EChatChannel::Zone;
	switch (CommandChar)
	{
	case 'a': Channel = EChatChannel::All; break;
	case 'f': Channel = EChatChannel::Friend; break;
	default: return false;
	}

	FString Message = Input.Mid(2).TrimStartAndEnd();
	if (!Message.IsEmpty())
	{
		NetMgr->SendMyChatMessage(Message, Channel);
		return true;
	}

	return false;
}

void UGPChatBoxWidget::ClearChatInput()
{
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
	UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (!NetMgr)
		return;
	bool bLoading = NetMgr->IsLoadingFromServer();
	if (bLoading)
	{
		//임시 - 로그인 후 잠시 시스템 메세지 출력 x 
		if(Channel == EChatChannel::FriendSys || Channel == EChatChannel::ItemSys || Channel == EChatChannel::SkillSys)
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