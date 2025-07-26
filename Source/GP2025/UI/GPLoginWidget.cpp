// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPLoginWidget.h"          
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"      
#include "Components/Widget.h"                        
#include "Kismet/GameplayStatics.h"  
#include "Kismet/KismetSystemLibrary.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Logging/LogMacros.h"      
#include "Player/GPPlayerController.h"
#include "Network/GPNetworkManager.h"

void UGPLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HideErrorMessage();

	if (TBInputID)
		TBInputID->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);
	if (TBInputPW)
		TBInputPW->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);

	if (TBSignUpID)
		TBSignUpID->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);
	if (TBSignUpPW)
		TBSignUpPW->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);
	if (TBSignUpNname)
		TBSignUpNname->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);

	if (ButtonLogin)
		ButtonLogin->OnClicked.AddDynamic(this, &UGPLoginWidget::TryLogin);
	if (ButtonSignUp)
		ButtonSignUp->OnClicked.AddDynamic(this, &UGPLoginWidget::SwitchWidget);
	if (ButtonExit)
		ButtonExit->OnClicked.AddDynamic(this, &UGPLoginWidget::OnExitClicked);

	if (ButtonSignUpCencle)
		ButtonSignUpCencle->OnClicked.AddDynamic(this, &UGPLoginWidget::SwitchWidget);
	if (ButtonSignUpOK)
		ButtonSignUpOK->OnClicked.AddDynamic(this, &UGPLoginWidget::TrySignUp);

	if (ConnectButton)
		ConnectButton->OnClicked.AddDynamic(this, &UGPLoginWidget::OnConnectButtonClicked);

	if (TextError && ConnectState && IpInputBox)
	{
		if (UGPNetworkManager* Mgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>())
		{
			Mgr->OnUserAuthFailed.AddDynamic(this, &UGPLoginWidget::HandleLoginFail);
			Mgr->OnConnectionResult.AddDynamic(this, &UGPLoginWidget::UpdateConnectState);
			bool Ret = Mgr->IsConnected();
			UpdateConnectState(Ret);
			IpInputBox->SetHintText(FText::FromString(FString(SERVER_IP)));

		}
	}
}

void UGPLoginWidget::SwitchWidget()
{
	if (LoginSwitcher)
	{
		int32 CurrentIndex = LoginSwitcher->GetActiveWidgetIndex();
		int32 NextIndex = (CurrentIndex == 0) ? 1 : 0;
		LoginSwitcher->SetActiveWidgetIndex(NextIndex);
	}

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
}

void UGPLoginWidget::OnEntered(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		if (!LoginSwitcher) return;

		int32 CurrentIndex = LoginSwitcher->GetActiveWidgetIndex();

		if (CurrentIndex == 0)
		{
			TryLogin();
		}
		else if (CurrentIndex == 1)
		{
			TrySignUp();
		}
	}
}

void UGPLoginWidget::OnExitClicked()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}

	APlayerController* PC = GetOwningPlayer();
	if (PC && GetWorld())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
	}
}

void UGPLoginWidget::HandleLoginFail(FString Message)
{
	if (!LoginSwitcher) return;

	int32 CurrentIndex = LoginSwitcher->GetActiveWidgetIndex();

	if (CurrentIndex == 0)
	{
		ShowLoginErrorMessage(Message, 3.0f);
		if (ButtonLogin)
			ButtonLogin->SetIsEnabled(true);
	}
	else if (CurrentIndex == 1)
	{
		ShowSignUpErrorMessage(Message, 3.0f);
		if (ButtonSignUpOK)
			ButtonSignUpOK->SetIsEnabled(true);
	}
}

void UGPLoginWidget::TryLogin()
{
	if (ButtonLogin)
		ButtonLogin->SetIsEnabled(false);
	FString ID_Str = TBInputID->GetText().ToString();
	FString PW_Str = TBInputPW->GetText().ToString();

	if (ID_Str.IsEmpty())
	{
		FString Msg = TEXT("아이디를 입력해주세요");
		ShowLoginErrorMessage(Msg, 3.0f);
		ButtonLogin->SetIsEnabled(true);
		return;
	}
	else if (PW_Str.IsEmpty())
	{
		FString Msg = TEXT("비밀번호를 입력해주세요");
		ShowLoginErrorMessage(Msg, 3.0f);
		ButtonLogin->SetIsEnabled(true);
		return;
	}
	auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	NetworkMgr->SendMyLoginPacket(ID_Str, PW_Str);

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
}

void UGPLoginWidget::TrySignUp()
{
	if (ButtonSignUpOK)
		ButtonSignUpOK->SetIsEnabled(false);
	FString ID_Str = TBSignUpID->GetText().ToString();
	FString PW_Str = TBSignUpPW->GetText().ToString();
	FString Nname_Str = TBSignUpNname->GetText().ToString();

	if (ID_Str.IsEmpty())
	{
		FString Msg = TEXT("아이디를 입력해주세요");
		ShowSignUpErrorMessage(Msg, 3.0f);
		ButtonSignUpOK->SetIsEnabled(true);
		return;
	}
	else if (PW_Str.IsEmpty())
	{
		FString Msg = TEXT("비밀번호를 입력해주세요");
		ShowSignUpErrorMessage(Msg, 3.0f);
		ButtonSignUpOK->SetIsEnabled(true);
		return;
	}
	else if (Nname_Str.IsEmpty())
	{
		FString Msg = TEXT("닉네임을 입력해주세요");
		ShowSignUpErrorMessage(Msg, 3.0f);
		ButtonSignUpOK->SetIsEnabled(true);
		return;
	}
	auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	NetworkMgr->SendMySignUpPacket(ID_Str, PW_Str, Nname_Str);

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
	}
}

void UGPLoginWidget::ShowLoginErrorMessage(const FString& Message, float Duration)
{
	if (!TextError) return;

	TextError->SetText(FText::FromString(Message));
	TextError->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(
		HideErrorTimerHandle,
		this,
		&UGPLoginWidget::HideErrorMessage,
		Duration,
		false
	);
}

void UGPLoginWidget::ShowSignUpErrorMessage(const FString& Message, float Duration)
{
	if (!TextSignUpError) return;

	TextSignUpError->SetText(FText::FromString(Message));
	TextSignUpError->SetVisibility(ESlateVisibility::Visible);

	GetWorld()->GetTimerManager().SetTimer(
		HideErrorTimerHandle,
		this,
		&UGPLoginWidget::HideErrorMessage,
		Duration,
		false
	);
}

void UGPLoginWidget::HideErrorMessage()
{
	if (TextError)
	{
		TextError->SetVisibility(ESlateVisibility::Hidden);
	}
	if (TextSignUpError)
	{
		TextSignUpError->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGPLoginWidget::OnConnectButtonClicked()
{
	FString InputIp = IpInputBox->GetText().ToString();
	if (InputIp.IsEmpty())
	{
		InputIp = SERVER_IP;
	}

	UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (NetMgr)
	{
		NetMgr->SetIpAddress(InputIp);
		NetMgr->TryConnectLoop();
	}
}

void UGPLoginWidget::UpdateConnectState(bool bConnected)
{
	if (!ConnectState) return;

	FLinearColor Color = bConnected ? FLinearColor(0.f, 1.f, 0.f, 0.5f) : FLinearColor(1.f, 0.f, 0.f, 0.5f);
	ConnectState->SetColorAndOpacity(FSlateColor(Color));
}