#pragma once
class PlayerSession;
class SessionSocket
{
public:
	SessionSocket(SOCKET socket);
	~SessionSocket();

	void Init(SOCKET& socket);
	void Disconnect();

	void DoRecv();
	void DoSend(const Packet* packet);

	void OnRecv(int32 id, int32 recvByte, ExpOver* expOver);
private:
	SOCKET _socket = INVALID_SOCKET;
	ExpOver _recvOver;
	uint32 _remain = 0;
};