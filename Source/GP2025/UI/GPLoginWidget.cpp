// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPLoginWidget.h"          
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"      
#include "Components/Widget.h"                        
#include "Kismet/GameplayStatics.h"  
#include "Kismet/KismetSystemLibrary.h"
#include "Components/Button.h"
#include "Logging/LogMacros.h"      
#include "Player/GPPlayerController.h"


void UGPLoginWidget::SendLoginDBPacket(bool isCA)
{
	// To Do : 패킷 처리하기 !!

// Protocol::C_LOGIN_DB Login_Pkt;

	auto ID = TCHAR_TO_UTF8(*ID_Str);
	auto PW = TCHAR_TO_UTF8(*PW_Str);

	// Login_Pkt.set_id(ID);
	// Login_Pkt.set_pw(PW);
	// Login_Pkt.set_iscreateaccount(isCA);

	// SEND_PACKET(Login_Pkt);
}

void UGPLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TBInputID)
		TBInputID->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);

	if (TBInputPW)
		TBInputPW->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);

	SetEnable(TextError, false);
	SetEnable(TextCreateAccount, true);
	SetEnable(TextLogin, true);

	if (ButtonLogin)
	{
		ButtonLogin->OnClicked.AddDynamic(this, &UGPLoginWidget::OnLoginClicked);
	}

	if (ButtonExit)
	{
		ButtonExit->OnClicked.AddDynamic(this, &UGPLoginWidget::OnExitClicked);
	}
}

void UGPLoginWidget::CreateAccount()
{
	SetEnable(TextCreateAccount, true);
	SetEnable(TextLogin, false);
	isCreate = true;
}

void UGPLoginWidget::CancleCreateAccount()
{
	SetEnable(TextCreateAccount, false);
	SetEnable(TextLogin, true);
	isCreate = false;
}

void UGPLoginWidget::OnEntered(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		ID_Str = TBInputID->GetText().ToString();
		PW_Str = TBInputPW->GetText().ToString();

		UE_LOG(LogTemp, Warning, TEXT("%s"), *ID_Str);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *PW_Str);

		if (isCreate)
			SendLoginDBPacket(isCreate);
		else
			SendLoginDBPacket(isCreate);
	}
}

void UGPLoginWidget::SetEnable(UWidget* widget, bool b)
{
	b ? widget->SetVisibility(ESlateVisibility::Visible) : widget->SetVisibility(ESlateVisibility::Hidden);
}

void UGPLoginWidget::OnLoginClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		AGPPlayerController* GPPC = Cast<AGPPlayerController>(PC);
		if (GPPC)
		{
			GPPC->ShowLobbyUI();  // 로비 UI 표시
		}
	}
}

void UGPLoginWidget::OnExitClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC && GetWorld())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
	}
}
// 서버 패킷 예
//message S_LOGIN_DB
//{
//	bool succes = 1;
//}
//
//message C_LOGIN_DB
//{
//	bool IsCreateAccount = 1;
//	string id = 2;
//	string pw = 3;
//}