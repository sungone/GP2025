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

	SetEnable(TextError, false);
	SetEnable(TextCreateAccount, true);
	SetEnable(TextLogin, true);

	//if (ButtonLogin)
	//{
	//	ButtonLogin->OnClicked.AddDynamic(this, &UGPLoginWidget::OnLoginClicked);
	//}

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

		if (ID_Str.IsEmpty() || PW_Str.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("ID or Password is Empty!"));
			return;
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("ID: %s | PW: %s"), *ID_Str, *PW_Str));
		if(!isCreate)
		{
			auto NetworkMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
			NetworkMgr->SendPlayerLoginPacket(TCHAR_TO_UTF8(*ID_Str), TCHAR_TO_UTF8(*PW_Str));
		}
	}
}

void UGPLoginWidget::SetEnable(UWidget* widget, bool b)
{
	b ? widget->SetVisibility(ESlateVisibility::Visible) : widget->SetVisibility(ESlateVisibility::Hidden);
}

//void UGPLoginWidget::OnLoginClicked()
//{
//	APlayerController* PC = GetOwningPlayer();
//	if (PC)
//	{
//		AGPPlayerController* GPPC = Cast<AGPPlayerController>(PC);
//		if (GPPC)
//		{
//			GPPC->ShowLobbyUI();  // 로비 UI 표시
//		}
//	}
//}

void UGPLoginWidget::OnExitClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC && GetWorld())
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
	}
}