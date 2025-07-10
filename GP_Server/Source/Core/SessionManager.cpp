#include "pch.h"
#include "SessionManager.h"
#include "PacketManager.h"

void SessionManager::Connect(SOCKET& socket)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	int32 _id = GenerateId();
	if (_id != -1) {
		_sessions[_id] = std::make_shared<PlayerSession>();
		_sessions[_id]->Connect(socket, _id);
		_iocp.RegisterSocket(socket, _id);
		_sessions[_id]->DoRecv();
	}
}

void SessionManager::Disconnect(int32 id)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	_sessions[id]->Disconnect();
	_sessions[id] = nullptr;
}

void SessionManager::Schedule(int32 sessionId, std::function<void()> job)
{
	auto session = GetSession(sessionId);
	if (session)
	{
		session->PushJob(job);
		GameJobScheduler::GetInst().Schedule(session);
	}
}

void SessionManager::GameJobWorkerLoop()
{
	while (true) {
		auto session = GameJobScheduler::GetInst().Pop();
		session->RunGameJobs();
	}
}

void SessionManager::DoRecv(int32 id)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	if (_sessions[id] == nullptr) return;
	_sessions[id]->DoRecv();
}

void SessionManager::HandleRecvBuffer(int32 id, int32 recvByte, ExpOver* expOver)
{
	_sessions[id]->HandleRecvBuffer(recvByte, expOver);
}

void SessionManager::HandleLogin(int32 sessionId, const DBLoginResult& dbRes)
{
	_sessions[sessionId]->Login(dbRes);
	auto& playerInfo = _sessions[sessionId]->GetPlayerInfo();
	SignUpSuccessPacket spkt;
	SendPacket(sessionId, &spkt);
}

void SessionManager::SendPacket(int32 sessionId, const Packet* packet)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	auto session = _sessions[sessionId];
	if (!session) 
	{
		LOG(Warning, "Invalid!"); 
		return; 
	}
	session->DoSend(packet);
}

void SessionManager::BroadcastToAll(Packet* packet)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	for (auto& session : _sessions)
	{
		if (!session || !session->IsLogin()) continue;
		session->DoSend(packet);
	}
}

void SessionManager::BroadcastToViewList(Packet* packet, const std::unordered_set<int32>& viewList)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	for (auto& session : _sessions)
	{
		if (!session || !session->IsLogin()) continue;
		int sid = session->GetId();
		if (viewList.count(sid))
			session->DoSend(packet);
	}
}

std::shared_ptr<PlayerSession> SessionManager::GetSession(int32 sessionId)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);

	if (sessionId < 0 || sessionId >= MAX_CLIENT)
		return nullptr;

	return _sessions[sessionId];
}


int SessionManager::GenerateId()
{
	for (int32 i = 1; i < MAX_CLIENT; ++i)
	{
		if (_sessions[i] != nullptr) continue;
		return i;
	}
	return -1;
}