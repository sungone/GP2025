#include "pch.h"
#include "SessionManager.h"
#include "PacketManager.h"

bool SessionManager::Init()
{
	for (int i = 1; i < MAX_CLIENT; ++i)
		_freeIds.push(i);
	return true;
}

void SessionManager::Connect(SOCKET& socket)
{
	std::lock_guard<std::mutex> lock(_sMutex);
	int32 _id = GenerateId();
	if (_id != -1) {
		_sessions[_id] = std::make_shared<PlayerSession>();
		_sessions[_id]->Connect(socket, _id);
		_iocp.RegisterSocket(socket, _id);
		_sessions[_id]->DoRecv();
	}
}

void SessionManager::Disconnect(int32 sessionId)
{
	auto session = GetSession(sessionId);
	if (!session)
	{
		LOG_W("Invalid");
		return;
	}

	session->Disconnect();
	session = nullptr;
	{
		std::lock_guard<std::mutex> idLock(_idMutex);
		_freeIds.push(sessionId);
	}
}

void SessionManager::Schedule(int32 sessionId, std::function<void()> job)
{
	auto session = GetSession(sessionId);
	if (!session)
	{
		LOG_W("Invalid");
		return;
	}
	session->PushJob(job);
	GameJobScheduler::GetInst().Schedule(session);
}

void SessionManager::GameJobWorkerLoop()
{
	while (true) {
		auto session = GameJobScheduler::GetInst().Pop();
		if (!session)
		{
			LOG_W("Invalid");
			return;
		}
		session->RunGameJobs();
	}
}

void SessionManager::DoRecv(int32 sessionId)
{
	auto session = GetSession(sessionId);
	if (!session)
	{
		LOG_W("Invalid");
		return;
	}
	session->DoRecv();
}

void SessionManager::OnRecv(int32 sessionId, int32 recvByte, ExpOver* expOver)
{
	auto session = GetSession(sessionId);
	if (!session)
	{
		LOG_W("Invalid");
		return;
	}
	session->OnRecv(recvByte, expOver);
}

void SessionManager::HandleLogin(int32 sessionId, const DBLoginResult& dbRes)
{
	auto session = GetSession(sessionId);
	if (!session)
	{
		LOG_W("Invalid");
		return;
	}
	session->Login(dbRes);
	auto& playerInfo = session->GetPlayerInfo();
	SignUpSuccessPacket spkt;
	SendPacket(sessionId, &spkt);
}

void SessionManager::SendPacket(int32 sessionId, const Packet* packet)
{
	auto session = GetSession(sessionId);
	if (!session)
	{
		LOG_W("Invalid");
		return;
	}
	session->DoSend(packet);
}

void SessionManager::OnSendCompleted(int32 sessionId, ExpOver* over)
{
	auto session = GetSession(sessionId);
	if (!session)
	{
		LOG_W("Invalid");
		return;
	}
	session->OnSendCompleted(over);
}

void SessionManager::BroadcastToAll(Packet* packet)
{
	std::vector<std::shared_ptr<PlayerSession>> snapshot;
	{
		std::lock_guard<std::mutex> lock(_sMutex);
		for (auto& session : _sessions)
		{
			if (session && session->IsLogin())
				snapshot.push_back(session);
		}
	}
	for (auto& session : snapshot)
		session->DoSend(packet);
}


void SessionManager::BroadcastToViewList(Packet* packet, const std::unordered_set<int32>& viewList)
{
	for (auto sessionId : viewList)
	{
		auto session = GetSession(sessionId);
		if (!session)
		{
			return;
		}
		session->DoSend(packet);
	}
}

std::shared_ptr<PlayerSession> SessionManager::GetSession(int32 sessionId)
{
	std::lock_guard<std::mutex> lock(_sMutex);

	if (sessionId < 0 || sessionId >= MAX_CLIENT)
		return nullptr;

	return _sessions[sessionId];
}

int SessionManager::GenerateId()
{
	std::lock_guard<std::mutex> lock(_idMutex);
	if (_freeIds.empty())
		return -1;

	int id = _freeIds.front();
	_freeIds.pop();
	return id;
}