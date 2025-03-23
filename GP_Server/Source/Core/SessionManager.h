#pragma once
#include "Session.h"
#include "IOCP.h"

class SessionManager
{
public:
	static SessionManager& GetInst()
	{
		static SessionManager inst;
		return inst;
	}
	void Connect(SOCKET& socket);
	void Disconnect(int32 sessionId);

	void DoRecv(int32 sessionId);
	void HandleRecvBuffer(int32 sessionId, int32 recvByte, ExpOver* expOver);
	void HandleLogin(int32 sessionId);
	void SendPacket(int32 sessionId, const Packet* packet);
	void BroadcastToAll(Packet* packet);
	void BroadcastToViewList(Packet* packet, int32 senderId);
	std::array<std::shared_ptr<Session>, MAX_CLIENT>& GetSessions() { return _sessions; }
private:
	int32 GenerateId();
private:
	std::array<std::shared_ptr<Session>, MAX_CLIENT> _sessions;
	IOCP& _iocp = IOCP::GetInst();

	std::mutex _smgrMutex;
};

