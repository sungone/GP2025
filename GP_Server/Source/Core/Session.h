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

	void Connect(SOCKET& socket, int32 id);
	void Disconnect();

	void SetLogin();
	bool IsLogin();
	int32 GetId();
	int32 GetRemainSize();
	void SetRemainSize(int32 size);
	FInfoData& GetPlayerInfo();
private:
	bool _bLogin = false;
	int32 _id = -1;
	std::shared_ptr<Player> _player;
	SOCKET _socket = INVALID_SOCKET;
	ExpOver _recvOver;
	int32 _remain;
	RWLock _sLock;
};
