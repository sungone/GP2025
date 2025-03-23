#include "pch.h"
#include "SessionManager.h"
#include "PacketManager.h"

void SessionManager::Connect(SOCKET& socket)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	int32 _id = GenerateId();
	if (_id != -1) {
		_sessions[_id] = std::make_shared<Session>();
		_sessions[_id]->Connect(socket, _id);
		_iocp.RegisterSocket(socket, _id);
		LOG("Start Recv");
		_sessions[_id]->DoRecv();
	}
}

void SessionManager::Disconnect(int32 id)
{
	std::lock_guard<std::mutex> lock(_smgrMutex);
	_sessions[id]->Disconnect();
	_sessions[id] = nullptr;
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

void SessionManager::HandleLogin(int32 id)
{
	auto session = _sessions[id];
	session->Login();
	auto& playerInfo = _sessions[id]->GetPlayerInfo();
	auto loginPkt = InfoPacket(EPacketType::S_LOGIN_SUCCESS, playerInfo);
	session->DoSend(&loginPkt);

	std::shared_ptr<Character> newPlayer = GameWorld::GetInst().GetCharacterByID(id);
	if (newPlayer)
	{
		GameWorld::GetInst().UpdateViewList(newPlayer);
	}
	GameWorld::GetInst().SpawnMonster(*session);

	//auto addPkt = InfoPacket(EPacketType::S_ADD_PLAYER, playerInfo);
	//BroadcastToViewList(&addPkt, id);

	std::lock_guard<std::mutex> lock(_smgrMutex);
	for (int32 otherId = 0; otherId < MAX_CLIENT; ++otherId)
	{
		if (otherId == id)continue;
		const auto& otherPlayerSession = _sessions[otherId];
		if (otherPlayerSession == nullptr || !otherPlayerSession->IsLogin()) continue;

		int32 otherPlayerId = otherPlayerSession->GetId();
		std::shared_ptr<Character> otherPlayer = GameWorld::GetInst().GetCharacterByID(otherPlayerId);

		if (otherPlayer && newPlayer->GetViewList().find(otherPlayerId) != newPlayer->GetViewList().end())
		{
			const auto& otherInfo = otherPlayerSession->GetPlayerInfo();
			const auto otherPlayerPkt = InfoPacket(EPacketType::S_ADD_PLAYER, otherInfo);
			session->DoSend(&otherPlayerPkt);
		}
	}
}

void SessionManager::SendPacket(int32 sessionId, const Packet* packet)
{
	if (!_sessions[sessionId]) { LOG(Warning, "Invalid!"); return; };
	_sessions[sessionId]->DoSend(packet);
}

void SessionManager::BroadcastToAll(Packet* packet)
{
}

void SessionManager::BroadcastToViewList(Packet* packet, int32 senderId)
{
	std::shared_ptr<Character> senderCharacter = GameWorld::GetInst().GetCharacterByID(senderId);
	if (!senderCharacter) return;
	GameWorld::GetInst().UpdateViewList(senderCharacter);
	const std::unordered_set<int32>& viewList = senderCharacter->GetViewList();

	std::lock_guard<std::mutex> lock(_smgrMutex);
	for (auto& session : _sessions)
	{
		if (session == nullptr || !session->IsLogin())
			continue;

		int32 sessionId = session->GetId();

		if (sessionId == senderId)
			continue;

		if (viewList.find(sessionId) != viewList.end())
		{
			session->DoSend(packet);
		}
	}
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