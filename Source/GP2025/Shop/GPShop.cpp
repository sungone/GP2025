// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop/GPShop.h"
#include "Components/TextBlock.h"  
#include "Components/Button.h"
#include "Character/GPCharacterNPC.h"

void UGPShop::NativeConstruct()
{
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UGPShop::OnShopExit);
	}
}

void UGPShop::OnShopExit()
{
	if (OwningNPC)
	{
		OwningNPC->ExitInteraction();
		RemoveFromParent();
	}
}
