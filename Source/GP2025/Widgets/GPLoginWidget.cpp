//// Fill out your copyright notice in the Description page of Project Settings.
//
//#include "Widgets/GPLoginWidget.h"
//#include "../../GP_Server/Proto.h"
//
//void UGPLoginWidget::SendLoginDBPacket(bool isCA)
//{
//	// ¼öÁ¤!!
//	//Protocol::C_LOGIN_DB Login_Pkt;
//
//	//auto ID = TCHAR_TO_UTF8(*ID_Str);
//	//auto PW = TCHAR_TO_UTF8(*PW_Str);
//
//	//Login_Pkt.set_id(ID);
//	//Login_Pkt.set_pw(PW);
//	//Login_Pkt.set_iscreateaccount(isCA);
//
//	//SEND_PACKET(Login_Pkt);
//}
//
//void UGPLoginWidget::NativeConstruct()
//{
//	//Super::NativeConstruct();
//
//	//if (TBInputID)
//	//{
//	//	TBInputID->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);
//	//}
//
//	//if (TBInputPW)
//	//{
//	//	TBInputPW->OnTextCommitted.AddDynamic(this, &UGPLoginWidget::OnEntered);
//	//}
//
//	//SetEnable(TextError, false);
//	//SetEnable(TextCreateAccount, false);
//	//SetEnable(TextLogin, true);
//}
//
//void UGPLoginWidget::CreateAccount()
//{
//	SetEnable(TextCreateAccount, true);
//	SetEnable(TextLogin, false);
//	isCreate = true;
//}
//
//void UGPLoginWidget::CancleCreateAccount()
//{
//	SetEnable(TextCreateAccount, false);
//	SetEnable(TextLogin, true);
//	isCreate = false;
//}
//
//void UGPLoginWidget::OnEntered(const FText& Text, ETextCommit::Type CommitMethod)
//{
//	if (CommitMethod == ETextCommit::OnEnter)
//	{
//		ID = TBInputID->GetText().ToString();
//		PW = TBInputPW->GetText().ToString();
//
//		UE_LOG(LogTemp, Warning, TEXT("%s"), *ID);
//		UE_LOG(LogTemp, Warning, TEXT("%s"), *PW);
//
//		if (isCreate)
//		{
//			SendLoginDBPacket(isCreate);
//		}
//		else
//		{
//			SendLoginDBPacket(isCreate);
//		}
//	}
//}
//
//void UGPLoginWidget::SetEnable(UWidget* Widget, bool b)
//{
//	b ? Widget->SetVisibility(ESlateVisibility::Visible) : Widget->SetVisibility(ESlateVisibility::Hidden);
//}
