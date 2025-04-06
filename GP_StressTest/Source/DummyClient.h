#pragma once
#include "ExpOver.h"

class DummyClient
{
public:
	DummyClient() {};
	~DummyClient() {};

	bool Init(uint32 num);
	bool Connect(IOCP& hIocp);
	void Disconnect();
	void DoRecv();
	void DoSend(Packet* packet);

	void HandleRecvBuffer(int32 recvByte, ExpOver* expOver);
	void ProcessPacket(Packet* packet);

	void SendLoginPacket();
	void SendSignUpPacket();
	void SendMovePacket();
private:
	SOCKET _socket = INVALID_SOCKET;
	ExpOver _recvOver;
	int32 _remain = 0;
	FInfoData _info;
	uint32 _dummyNum = 0;
	std::string _name;
	int32& _playerId = _info.ID;
	std::thread _moveThread;
};

