#pragma once
#include "Define.h"
#include "Player.h"

class Session
{
public:
	Session() = default;
	~Session() { closesocket(socket); }

public:
	void DoRecv();
	void DoSend(Packet* packet);

	void Login();
	void Disconnect();

public:
	bool bLogin = false;
	FStatusData info;
	int32& id = info.ID;
	SOCKET socket = INVALID_SOCKET;
	ExpOver recvOver;
	int32 remain;
};
