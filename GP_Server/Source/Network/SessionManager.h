#pragma once
#include "Define.h"
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
	void Broadcast(Packet* packet, int32 exptId = -1);
	int GenerateId();

	std::array<Session, MAX_CLIENT>& GetSessions() { return sessions; }
private:
	std::array<Session, MAX_CLIENT> sessions;
	IOCP& iocp = IOCP::GetInst();
};

