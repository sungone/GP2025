// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Modules/GPCharacterUIHandler.h"
#include "Character/GPCharacterBase.h"
#include "Character/GPCharacterMonster.h"
#include "UI/GPWidgetComponent.h"
#include "UI/GPHpBarWidget.h"
#include "UI/GPLevelWidget.h"
#include "UI/GPUserNameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


void UGPCharacterUIHandler::Initialize(AGPCharacterBase* InOwner)
{
	Owner = InOwner;
	CreateAllWidgets();
}

void UGPCharacterUIHandler::OnBeginPlay()
{
	SetupNickNameUI();
}

void UGPCharacterUIHandler::CreateAllWidgets()
{
	NickNameText = CreateWidgetComponent(TEXT("NickNameWidget"), TEXT("/Game/UI/WBP_UserName"), FVector(0.f, 0.f, 330.f), FVector2D(200.f, 0.f), NickNameWidget);

	if (Owner && Owner->IsA(AGPCharacterMonster::StaticClass()))
	{
		NickNameText->SetRelativeLocation(FVector(0.f, 0.f, 360.f));
		HpBar = CreateWidgetComponent(TEXT("HpBarWidget"), TEXT("/Game/UI/WBP_CharacterHpBar"), FVector(0.f, 0.f, 320.f), FVector2D(200.f, 30.f), HpBarWidget);
		LevelText = CreateWidgetComponent(TEXT("LevelTextWidget"), TEXT("/Game/UI/WBP_LevelText"), FVector(0.f, 0.f, 395.f), FVector2D(100.f, 40.f), LevelTextWidget);
	}

	if (NickNameWidget)
	{
		SetupNickNameUI();
	}
}


void UGPCharacterUIHandler::SetupNickNameUI()
{
	if (!Owner || !NickNameText)
		return;
	

	UUserWidget* Widget = NickNameText->GetUserWidgetObject();
	if (!Widget)
		return;
	

	UGPUserNameWidget* NameWidget = Cast<UGPUserNameWidget>(Widget);
	if (!NameWidget)
		return;
	

	FString NickName = FString(UTF8_TO_TCHAR(Owner->CharacterInfo.GetName()));
	NameWidget->UpdateNickNameText(NickName);

	if (NickNameWidget)
	{
		NickNameWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

UGPWidgetComponent* UGPCharacterUIHandler::CreateWidgetComponent(
	const FString& Name,
	const FString& WidgetPath,
	FVector Location,
	FVector2D Size,
	UUserWidget*& OutUserWidget)
{
	UGPWidgetComponent* WidgetComp = NewObject<UGPWidgetComponent>(Owner, *Name);
	WidgetComp->RegisterComponent();
	WidgetComp->AttachToComponent(Owner->GetCharacterMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	WidgetComp->SetComponent(Location, Size);

	FString ClassPath = WidgetPath + TEXT(".") + FPaths::GetCleanFilename(WidgetPath) + TEXT("_C");

	TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(nullptr, *ClassPath);
	if (WidgetClass)
	{
		WidgetComp->SetWidgetClass(WidgetClass);
		WidgetComp->InitWidget();
		OutUserWidget = WidgetComp->GetUserWidgetObject();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UIHandler] Failed to load: %s"), *ClassPath);
	}

	return WidgetComp;
}

void UGPCharacterUIHandler::UpdateAllWidgetVisibility()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(Owner->GetWorld(), 0);
	AGPCharacterBase* LocalPlayer = Cast<AGPCharacterBase>(PC ? PC->GetPawn() : nullptr);
	if (!LocalPlayer) return;

	float Distance = FVector::Dist(Owner->GetActorLocation(), LocalPlayer->GetActorLocation());
	bool bVisible = Distance <= Owner->CharacterInfo.CollisionRadius + LocalPlayer->CharacterInfo.AttackRadius;

	ESlateVisibility Visibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

	if (HpBarWidget) HpBarWidget->SetVisibility(Visibility);
	if (LevelTextWidget) LevelTextWidget->SetVisibility(Visibility);
	if (NickNameWidget) NickNameWidget->SetVisibility(Visibility);
}

