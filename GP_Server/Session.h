#pragma once
#include "Define.h"

class Session {

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
	FPlayerInfo info;
	int32& id = info.ID;
	SOCKET socket = INVALID_SOCKET;
	ExpOver recvOver;
	int32 remain;
};
