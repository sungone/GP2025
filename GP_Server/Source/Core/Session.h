#pragma once
#include "Player.h"
#include "SessionSocket.h"

class Session
{
public:
	Session() = default;
	~Session() = default;

public:
	void DoRecv();
	void DoSend(Packet* packet);

	void Connect(SOCKET socket, int32 id);
	void Disconnect();

	void SetLogin();
	bool IsLogin();
	int32 GetId();
	FInfoData& GetPlayerInfo();
	
	void HandleRecvBuffer(int32 recvByte, ExpOver* expOver);
private:
	bool _bLogin = false;
	int32 _id = -1;
	std::shared_ptr<Player> _player;
	std::unique_ptr<SessionSocket> _sSocket;

	std::mutex _sMutex;
};
