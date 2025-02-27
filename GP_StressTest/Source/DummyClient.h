#pragma once
#include "ExpOver.h"

class DummyClient
{
public:
	DummyClient();
	~DummyClient();

	bool Connect();
	void Disconnect();
	void DoRecv();
	void DoSend(Packet&& packet);
	void HandleRecvBuffer(int32 recvByte, ExpOver* expOver);
	void ProcessPacket(BYTE* packet);
	static void Move();
	SOCKET& GetSocket() { return _socket; }

private:
	SOCKET _socket = INVALID_SOCKET;
	ExpOver _recvOver;
	int32 _remain = 0;
	FInfoData _info;
};

