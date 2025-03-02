// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GPMoney.h"

// Sets default values
AGPMoney::AGPMoney()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGPMoney::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGPMoney::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

