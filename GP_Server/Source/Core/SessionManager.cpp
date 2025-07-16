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
		_sessions[_id] = std::make_unique<PlayerSession>();
		_sessions[_id]->Connect(socket, _id);
		_iocp.RegisterSocket(socket, _id);
		_sessions[_id]->DoRecv();
	}
}

void SessionManager::Disconnect(int32 sessionId)
{
	{
		std::lock_guard<std::mutex> lock(_sMutex);

		if (sessionId < 0 || sessionId >= MAX_CLIENT || !_sessions[sessionId])
		{
			LOG_W("Invalid session id: {}", sessionId);
			return;
		}

		_sessions[sessionId]->Disconnect();
		_sessions[sessionId].reset();
	}

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
	GameJobScheduler::GetInst().Schedule(sessionId);
}

void SessionManager::GameJobWorkerLoop()
{
	while (true) {
		int32 sessionId = GameJobScheduler::GetInst().Pop();
		auto session = GetSession(sessionId);
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
	if (!dbRes.friends.empty())
	{
		const auto& friends = dbRes.friends;
		for (const FFriendInfo& f : friends)
		{
			if (f.bAccepted)
			{
				AddFriendPacket pkt(f);
				SessionManager::GetInst().SendPacket(sessionId, &pkt);
			}
			else if (!f.bIsRequester)
			{
				FriendRequestPacket requestPkt(f);
				SessionManager::GetInst().SendPacket(sessionId, &requestPkt);
			}
		}
	}
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

void SessionManager::BroadcastToAll(Packet* packet)
{
	for (auto& session : _sessions)
		if (session && session->IsLogin())
			session->DoSend(packet);
}

void SessionManager::BroadcastToFriends(int32 sessionId, Packet* packet)
{
	auto session = GetSession(sessionId);
	if (!session)
	{
		LOG_W("Invalid");
		return;
	}
	const auto& friends = session->GetFriends();
	for (const auto& f : friends)
	{
		int32 fsessId = GetOnlineSessionIdByDBId(f.DBId);
		if (fsessId != -1)
		{
			SendPacket(fsessId, packet);
		}
	}
}

void SessionManager::BroadcastToViewList(Packet* packet, const std::unordered_set<int32>& viewList)
{
	for (auto sessionId : viewList)
	{
		if (sessionId > MAX_PLAYER) continue;

		auto session = GetSession(sessionId);
		if (!session)
		{
			return;
		}
		session->DoSend(packet);
	}
}

PlayerSession* SessionManager::GetSession(int32 sessionId)
{
	std::lock_guard<std::mutex> lock(_sMutex);

	if (sessionId < 0 || sessionId >= MAX_CLIENT)
		return nullptr;

	return _sessions[sessionId].get();
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

int32 SessionManager::GetOnlineSessionIdByDBId(uint32 dbid)
{
	for (auto& session : _sessions)
	{
		if (session && session->IsLogin() && session->GetUserDBID() == dbid)
			return session->GetId();
	}
	return -1;
}