#include "Character/Modules/GPCharacterUIHandler.h"
#include "Character/GPCharacterMyplayer.h"
#include "UI/GPWidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "UI/GPHpBarWidget.h"
#include "UI/GPLevelWidget.h"
#include "UI/GPUserNameWidget.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Character/GPCharacterMonster.h"
#include "Character/Modules/GPMyplayerCameraHandler.h"
#include "UI/GPCharacterStatusWidget.h"
#include "GameFramework/PlayerController.h"

void UGPCharacterUIHandler::Initialize(AGPCharacterBase* InOwner)
{
	Owner = InOwner;
	CreateCharacterStatusWidget();

	if (Cast<AGPCharacterPlayer>(Owner))
	{
		UGPCharacterStatusWidget* StatusWidget = Cast<UGPCharacterStatusWidget>(CharacterStatusWidgetInstance);
		if (StatusWidget && StatusWidget->WBPHpBar)
		{
			StatusWidget->WBPHpBar->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UGPCharacterUIHandler::CreateCharacterStatusWidget()
{
	CharacterStatusWidget = CreateWidgetComponent(
		TEXT("CharacterStatusWidget"),
		TEXT("/Game/UI/WBP_CharacterStatus"),
		FVector(0.f, 0.f, 320.f),
		FVector2D(280.f, 80.f),
		CharacterStatusWidgetInstance
	);
}

void UGPCharacterUIHandler::UpdateWidgetVisibility()
{
	if (!Owner) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(Owner->GetWorld(), 0);
	AGPCharacterBase* LocalPlayer = Cast<AGPCharacterBase>(PC ? PC->GetPawn() : nullptr);
	if (!LocalPlayer) return;

	AGPCharacterMyplayer* LocalMyPlayer = Cast<AGPCharacterMyplayer>(LocalPlayer);
	if (!LocalMyPlayer) return;

	const bool bIsOwnerSelf = (Owner == LocalMyPlayer);
	const bool bIsGunner = LocalMyPlayer->bIsGunnerCharacter();
	const bool bIsGunnerZooming = bIsGunner && LocalMyPlayer->CameraHandler->IsZooming();

	if (bIsOwnerSelf)
	{
		CharacterStatusWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	const float Distance = FVector::Dist(Owner->GetActorLocation(), LocalMyPlayer->GetActorLocation());

	float MaxVisibleDistance = 500.0f; // 기본 거리

	if (bIsGunner && bIsGunnerZooming)
	{
		MaxVisibleDistance = LocalMyPlayer->CharacterInfo.AttackRadius;

		const float MinScaleDistance = 300.0f;
		const float MaxScaleDistance = 5000.0f;
		const float DistanceFactor = FMath::Clamp((MaxScaleDistance - Distance) / (MaxScaleDistance - MinScaleDistance), 0.0f, 1.0f);
		const float ScaleFactor = FMath::Lerp(0.0001f, 1.f, DistanceFactor);
		CharacterStatusWidget->SetRelativeScale3D(FVector(ScaleFactor));
	}
	else
	{
		CharacterStatusWidget->SetRelativeScale3D(FVector(1.f));
	}

	const float VisibleRadius = Owner->CharacterInfo.CollisionRadius + MaxVisibleDistance;
	const bool bVisible = Distance <= VisibleRadius;

	CharacterStatusWidgetInstance->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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

void UGPCharacterUIHandler::SetNameByCharacterInfo()
{
	if (!CharacterStatusWidgetInstance || !Owner) return;

	UGPCharacterStatusWidget* StatusWidget = Cast<UGPCharacterStatusWidget>(CharacterStatusWidgetInstance);
	if (StatusWidget && StatusWidget->WBPName)
	{
		FString Nick = FString(UTF8_TO_TCHAR(Owner->CharacterInfo.GetName()));
		StatusWidget->WBPName->UpdateNickNameText(Nick);
	}
}