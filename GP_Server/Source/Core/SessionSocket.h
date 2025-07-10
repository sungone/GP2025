#pragma once
class PlayerSession;
class SessionSocket
{
public:
	SessionSocket(SOCKET socket);
	~SessionSocket();

	void DoRecv();
	void DoSend(const Packet* packet);

	void Init(SOCKET& socket);
	void Close();

	void HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver);
	void SetOwnerSession(PlayerSession* session) { _owner = session; }
	PlayerSession* GetSession() const { return _owner; }
private:
	SOCKET _socket = INVALID_SOCKET;
	ExpOver _recvOver;
	uint32 _remain = 0;
	PlayerSession* _owner = nullptr;
};