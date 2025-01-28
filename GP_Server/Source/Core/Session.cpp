#include "pch.h"
#include "Session.h"
#include "SessionManager.h"
#include "GameManager.h"

void Session::DoRecv()
{
	ZeroMemory(&_recvOver._wsaover, sizeof(_recvOver._wsaover));
	DWORD recv_flag = 0;
	_recvOver._wsabuf.len = BUFSIZE - _remain;
	_recvOver._wsabuf.buf = _recvOver._buf + _remain;
	WSARecv(_socket, &_recvOver._wsabuf, 1, 0, &recv_flag, &_recvOver._wsaover, 0);
}
void Session::DoSend(Packet* packet)
{
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

void Session::Login()
{
	_bLogin = true;
}

void Session::Connect(SOCKET& socket, int32 id)
{
	this->_id = id;
	this->_socket = socket;

	_player = std::make_shared<Player>();
	_player->Init();
	_player->GetInfo().ID = id;
}

void Session::Disconnect()
{
	_bLogin = false;
	closesocket(_socket);
}
