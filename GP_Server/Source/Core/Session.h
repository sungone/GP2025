#pragma once
#include "Player.h"

class Session
{
public:
	Session() = default;
	~Session() { closesocket(_socket); }

public:
	void DoRecv();
	void DoSend(Packet* packet);

	void Login();
	void Connect(SOCKET& socket, int32 id);
	void Disconnect();

public:
	bool _bLogin = false;
	int _id = -1;
	std::shared_ptr<Player> _player;
	SOCKET _socket = INVALID_SOCKET;
	ExpOver _recvOver;
	int32 _remain;
};
