#pragma once
class PlayerSession;
class SessionSocket
{
public:
	SessionSocket(SOCKET socket);
	~SessionSocket();

	void Init(SOCKET& socket);
	void Shutdown();

	void DoRecv();
	void DoSend(const Packet* packet);

	void OnRecv(int32 id, int32 recvByte, ExpOver* expOver);
	void OnSendCompleted(ExpOver* over);
private:
	SOCKET _socket = INVALID_SOCKET;
	ExpOver _recvOver;
	uint32 _remain = 0;
};