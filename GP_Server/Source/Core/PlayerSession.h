#pragma once
#include "Player.h"
#include "SessionSocket.h"

class PlayerSession
{
public:
	PlayerSession() = default;
	~PlayerSession()
	{
		if(IsLogin()) Disconnect();
	}

public:
	void DoRecv();
	void DoSend(const Packet* packet);

	void Connect(SOCKET socket, int32 id);
	void Disconnect();

	void Login(const DBLoginResult& dbRes);
	void Logout();
	bool IsLogin() { return _loginState; }

	int32 GetId();
	FInfoData& GetPlayerInfo();
	
	void HandleRecvBuffer(int32 recvByte, ExpOver* expOver);
private:
	int32 _id = -1;
	uint32 _dbId;
	std::shared_ptr<Player> _player;
	std::unique_ptr<SessionSocket> _sSocket;
	bool _loginState = false;

	std::mutex _sMutex;
};
