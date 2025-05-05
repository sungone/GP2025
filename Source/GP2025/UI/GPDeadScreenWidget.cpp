// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GPDeadScreenWidget.h"
#include "Character/GPCharacterMyplayer.h"
#include "Kismet/GameplayStatics.h"
#include "Network/GPNetworkManager.h"

void UGPDeadScreenWidget::OnRespawnButtonClicked()
{
	UGPNetworkManager* NetMgr = GetGameInstance()->GetSubsystem<UGPNetworkManager>();
	if (NetMgr)
	{
		NetMgr->SendMyRespawnPacket(ZoneType::TIP);
	}
}
