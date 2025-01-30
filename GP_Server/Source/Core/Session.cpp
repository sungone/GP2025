#include "pch.h"
#include "Session.h"
#include "SessionManager.h"
#include "GameManager.h"

void Session::DoRecv()
{
	RWLock::WriteGuard guard(_sLock);
	ZeroMemory(&_recvOver._wsaover, sizeof(_recvOver._wsaover));
	DWORD recv_flag = 0;
	_recvOver._wsabuf.len = BUFSIZE - _remain;
	_recvOver._wsabuf.buf = _recvOver._buf + _remain;
	WSARecv(_socket, &_recvOver._wsabuf, 1, 0, &recv_flag, &_recvOver._wsaover, 0);
}
void Session::DoSend(Packet* packet)
{
	RWLock::WriteGuard guard(_sLock);

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
		LOG(LogType::SendLog, std::format("Monsterpdate PKT to [{}]", _id));
		break;
	default:
		LOG(LogType::SendLog, "Unknown Packet Type");
		break;
	}
#pragma endregion
	auto send_data = new ExpOver{ reinterpret_cast<BYTE*>(packet) };
	WSASend(_socket, &send_data->_wsabuf, 1, nullptr, 0, &send_data->_wsaover, nullptr);
}

void Session::Connect(SOCKET& socket, int32 id)
{
	RWLock::WriteGuard guard(_sLock);
	this->_id = id;
	this->_socket = socket;

	_player = std::make_shared<Player>();
	_player->Init();
	_player->GetInfo().ID = id;
}

void Session::Disconnect()
{
	RWLock::WriteGuard guard(_sLock);
	_bLogin = false;
	closesocket(_socket);
}

void Session::SetLogin()
{
	RWLock::WriteGuard guard(_sLock);
	_bLogin = true;
}

bool Session::IsLogin()
{
	RWLock::ReadGuard guard(_sLock);
	return _bLogin;
}

int32 Session::GetId()
{
	RWLock::ReadGuard guard(_sLock);
	return _id;
}

int32 Session::GetRemainSize()
{
	RWLock::ReadGuard guard(_sLock);
	return _remain;
}

void Session::SetRemainSize(int32 size)
{
	RWLock::WriteGuard guard(_sLock);
	_remain = size;
}

FInfoData& Session::GetPlayerInfo()
{
	RWLock::ReadGuard guard(_sLock);
	return _player->GetInfo();
}
