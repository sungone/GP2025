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
	void Connect(SOCKET& socket, int32 id);
	void Disconnect();

public:
	bool bLogin = false;
	int id = -1;
	std::shared_ptr<Player> player;
	SOCKET socket = INVALID_SOCKET;
	ExpOver recvOver;
	int32 remain;
};
