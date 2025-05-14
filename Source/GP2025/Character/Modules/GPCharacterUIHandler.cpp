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
	bool bIsGunnerZooming = LocalMyPlayer && LocalMyPlayer->bIsGunnerCharacter() && LocalMyPlayer->CameraHandler->IsZooming();

	if (LocalMyPlayer && LocalMyPlayer->bIsGunnerCharacter() && !LocalMyPlayer->CameraHandler->IsZooming())
	{
		CharacterStatusWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	float Distance = FVector::Dist(Owner->GetActorLocation(), LocalPlayer->GetActorLocation());
	float AttackRadius = 300.0f; 

	if (LocalMyPlayer)
	{
		AttackRadius = LocalMyPlayer->CharacterInfo.AttackRadius;
	}

	if (bIsGunnerZooming)
	{
		float MinScaleDistance = 300.0f;  // 최대 크기 거리
		float MaxScaleDistance = 5000.0f; // 최소 크기 거리

		float DistanceFactor = FMath::Clamp((Distance - MinScaleDistance) / (MaxScaleDistance - MinScaleDistance), 0.0f, 1.0f);

		// ✅ 가까울 때 커지고, 멀리 있을 때 작아지도록 설정
		float ScaleFactor = FMath::Clamp(
			2.0f - DistanceFactor * 1.9f,  // 2.0f에서 0.1f까지 감소
			0.1f,  // 최소 크기 (멀리 있을 때)
			2.0f   // 최대 크기 (가까이 있을 때)
		);

		CharacterStatusWidget->SetRelativeScale3D(FVector(ScaleFactor, ScaleFactor, ScaleFactor));
	}
	else
	{
		CharacterStatusWidget->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}

	bool bVisible = Distance <= (Owner->CharacterInfo.CollisionRadius + AttackRadius);
	ESlateVisibility Visibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	CharacterStatusWidgetInstance->SetVisibility(Visibility);
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