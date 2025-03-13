#pragma once
class Session;
class SessionSocket
{
public:
	SessionSocket(SOCKET socket);
	~SessionSocket();

	void DoRecv();
	void DoSend(Packet* packet);

	void Init(SOCKET& socket);
	void Close();

	void HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver);
private:
	SOCKET _socket = INVALID_SOCKET;
	ExpOver _recvOver;
	int32 _remain = 0;
};