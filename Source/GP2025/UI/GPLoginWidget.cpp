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
#include "Network/GPNetworkManager.h"

void UGPLoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TBInputID)
		TBInputID->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);

	if (TBInputPW)
		TBInputPW->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);

	TextError->SetVisibility(ESlateVisibility::Hidden);

	if (ButtonLogin)
	{
		ButtonLogin->OnClicked.AddDynamic(this, &UGPLoginWidget::OnLoginClicked);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("?!")));
	}
	if (ButtonSignUp)
	{
		ButtonSignUp->OnClicked.AddDynamic(this, &UGPLoginWidget::OnSignUpClicked);
	}
	if (ButtonExit)
	{
		ButtonExit->OnClicked.AddDynamic(this, &UGPLoginWidget::OnExitClicked);
	}

	if (UGPNetworkManager* Mgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>())
	{
		Mgr->OnLoginFailed.AddDynamic(this, &UGPLoginWidget::HandleLoginFail);
	}
}

void UGPLoginWidget::OnEntered(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		TryLogin();
	}
}

void UGPLoginWidget::OnLoginClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("OnLoginClicked")));
	TryLogin();
}

void UGPLoginWidget::OnSignUpClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("OnSignUpClicked")));
	TrySignUp();
}

void UGPLoginWidget::OnExitClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC && GetWorld())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
	}
}

void UGPLoginWidget::HandleLoginFail(FString Message)
{
	ShowErrorMessage(Message, 3.0f);
}

void UGPLoginWidget::TryLogin()
{
	ID_Str = TBInputID->GetText().ToString();
	PW_Str = TBInputPW->GetText().ToString();

	if (ID_Str.IsEmpty())
	{
		FString Msg = TEXT("아이디를 입력해주세요");
		ShowErrorMessage(Msg, 3.0f);
		return;
	}
	else if (PW_Str.IsEmpty())
	{
		FString Msg = TEXT("비밀번호를 입력해주세요");
		ShowErrorMessage(Msg, 3.0f);
		return;
	}
	auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	NetworkMgr->SendPlayerLoginPacket(TCHAR_TO_UTF8(*ID_Str), TCHAR_TO_UTF8(*PW_Str));
}

void UGPLoginWidget::TrySignUp()
{
	//Todo: 회원가입 UI 
}

void UGPLoginWidget::ShowErrorMessage(const FString& Message, float Duration)
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

void UGPLoginWidget::HideErrorMessage()
{
	if (TextError)
	{
		TextError->SetVisibility(ESlateVisibility::Hidden);
	}
}