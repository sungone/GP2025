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
	void Disconnect(int32 id);

	void DoRecv(int32 id);
	void HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver);
	void HandleLogin(int32 id);
	void Broadcast(Packet* packet, int32 exptId = -1);
	std::array<std::shared_ptr<Session>, MAX_CLIENT>& GetSessions() { return _sessions; }
private:
	int32 GenerateId();
private:
	std::array<std::shared_ptr<Session>, MAX_CLIENT> _sessions;
	IOCP& _iocp = IOCP::GetInst();

	std::mutex _smgrMutex;
};

