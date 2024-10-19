// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FSocket;

class GP2025_API GPRecvThread :public FRunnable
{
public:
	GPRecvThread(FSocket* Socket);
	~GPRecvThread() {};
	
	virtual uint32 Run() override;
	void Destroy() { isThreadRuning = false; };

private:
	FRunnableThread* Thread = nullptr;
	FSocket* Socket;
	bool isThreadRuning = true;
};