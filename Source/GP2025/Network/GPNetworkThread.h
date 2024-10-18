// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FSocket;

class GP2025_API GPNetworkThread :public FRunnable
{
public:
	GPNetworkThread(FSocket* Socket);
	~GPNetworkThread() {};
	
	virtual uint32 Run() override;
	void Destroy() { isThreadRuning = false; };

private:
	FRunnableThread* Thread = nullptr;
	FSocket* Socket;
	bool isThreadRuning = true;
};