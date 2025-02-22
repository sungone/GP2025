#include "pch.h"
#include "Session.h"
#include "SessionManager.h"
#include "GameManager.h"
#include "PacketManager.h"

void Session::DoRecv()
{
	_sSocket->DoRecv();
}
void Session::DoSend(Packet* packet)
{
	std::lock_guard<std::mutex> lock(_sMutex);

#pragma region //Log
	switch (packet->Header.PacketType)
	{
	case S_LOGIN_SUCCESS:
		LOG(LogType::SendLog, std::format("LoginInfo PKT to [{}]", _id));
		break;
	case S_ADD_PLAYER:
		LOG(LogType::SendLog, std::format("AddPlayer PKT to [{}]", _id));
		break;
	case S_REMOVE_PLAYER:
		LOG(LogType::SendLog, std::format("RemovePlayer PKT to [{}]", _id));
		break;
	case S_PLAYER_STATUS_UPDATE:
		LOG(LogType::SendLog, std::format("PlayerUpdate PKT to [{}]", _id));
		break;
	case S_ADD_MONSTER:
		LOG(LogType::SendLog, std::format("AddMonster PKT to [{}]", _id));
		break;
	case S_REMOVE_MONSTER:
		LOG(LogType::SendLog, std::format("RemoveMonster PKT to [{}]", _id));
		break;
	case S_MONSTER_STATUS_UPDATE:
		//LOG(LogType::SendLog, std::format("MonsterUpdate PKT to [{}]", _id));
		break;
	case S_DAMAGED_MONSTER:
		LOG(LogType::SendLog, std::format("DamagedMonster PKT to [{}]", _id));
		break;
	default:
		LOG(LogType::SendLog, "Unknown Packet Type");
		break;
	}
#pragma endregion
	_sSocket->DoSend(packet);
}

void Session::Connect(SOCKET socket, int32 id)
{
	std::lock_guard<std::mutex> lock(_sMutex);
	this->_id = id;
	_sSocket = std::make_unique<SessionSocket>(socket);
}

void Session::Disconnect()
{
	std::lock_guard<std::mutex> lock(_sMutex);
	_bLogin = false;
	_sSocket->Close();
}

void Session::SetLogin()
{
	std::lock_guard<std::mutex> lock(_sMutex);
	_bLogin = true;
	_player = std::make_shared<Player>();
	_player->Init();
	_player->GetInfo().ID = _id;
	GameManager::GetInst().AddPlayer(_player);
}

bool Session::IsLogin()
{
	std::lock_guard<std::mutex> lock(_sMutex);
	return _bLogin;
}

int32 Session::GetId()
{
	std::lock_guard<std::mutex> lock(_sMutex);
	return _id;
}

FInfoData& Session::GetPlayerInfo()
{
	std::lock_guard<std::mutex> lock(_sMutex);
	return _player->GetInfo();
}

void Session::HandleRecvBuffer(int32 recvByte, ExpOver* expOver)
{
	_sSocket->HandleRecvBuffer(*this, recvByte, expOver);
}