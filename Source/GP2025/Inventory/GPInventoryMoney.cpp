// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/GPInventoryMoney.h"

// Sets default values
AGPInventoryMoney::AGPInventoryMoney()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGPInventoryMoney::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGPInventoryMoney::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

