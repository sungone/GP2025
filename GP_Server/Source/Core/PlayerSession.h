#pragma once
#include "Player.h"
#include "SessionSocket.h"

class PlayerSession
{
public:
	PlayerSession() = default;
	~PlayerSession() = default;

public:
	void DoRecv();
	void DoSend(const Packet* packet);

	void Connect(SOCKET socket, int32 id);
	void Disconnect();

	void Login();
	void CreatePlayer();
	bool IsLogin() { return _loginState; }

	int32 GetId();
	FInfoData& GetPlayerInfo();
	
	void HandleRecvBuffer(int32 recvByte, ExpOver* expOver);
private:
	int32 _id = -1;
	std::shared_ptr<Player> _player;
	std::unique_ptr<SessionSocket> _sSocket;
	bool _loginState = false;

	std::mutex _sMutex;
};
