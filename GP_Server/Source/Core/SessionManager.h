#pragma once
#include "PlayerSession.h"
#include "IOCP.h"
#include "GameJobScheduler.h"
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
	void GameJobWorkerLoop();
	void Schedule(int32 sessionId, std::function<void()> job);

	void DoRecv(int32 sessionId);
	void OnRecv(int32 sessionId, int32 recvByte, ExpOver* expOver);
	void HandleLogin(int32 sessionId, const DBLoginResult& dbRes = DBLoginResult());
	void SendPacket(int32 sessionId, const Packet* packet);
	void OnSendCompleted(int32 sessionId, ExpOver* over);
	void BroadcastToAll(Packet* packet);
	void BroadcastToViewList(Packet* packet, const std::unordered_set<int32>& viewList);
	std::array<std::shared_ptr<PlayerSession>, MAX_CLIENT>& GetSessions() { return _sessions; }
	std::shared_ptr<PlayerSession> GetSession(int32 sessionId);

private:
	int32 GenerateId();
private:
	std::array<std::shared_ptr<PlayerSession>, MAX_CLIENT> _sessions;
	IOCP& _iocp = IOCP::GetInst();
	GameJobScheduler& _jobManager = GameJobScheduler::GetInst();

	std::mutex _smgrMutex;
};

