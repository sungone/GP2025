#include "Session.h"
#include "SessionManager.h"
#include "GameManager.h"

void Session::DoRecv()
{
	ZeroMemory(&recvOver.wsaover, sizeof(recvOver.wsaover));
	DWORD recv_flag = 0;
	recvOver.wsabuf.len = BUFSIZE - remain;
	recvOver.wsabuf.buf = recvOver.buf + remain;
	WSARecv(socket, &recvOver.wsabuf, 1, 0, &recv_flag, &recvOver.wsaover, 0);
}
void Session::DoSend(Packet* packet)
{
#pragma region //Log
	switch (packet->Header.PacketType)
	{
	case S_LOGIN_SUCCESS:
		LOG(LogType::SendLog, std::format("LoginInfo PKT to [{}]", id));
		break;
	case S_ADD_PLAYER:
		LOG(LogType::SendLog, std::format("AddPlayer PKT to [{}]", id));
		break;
	case S_REMOVE_PLAYER:
		LOG(LogType::SendLog, std::format("RemovePlayer PKT to [{}]", id));
		break;
	case S_PLAYER_STATUS_UPDATE:
		LOG(LogType::SendLog, std::format("PlayerUpdate PKT to [{}]", id));
		break;
	case S_ADD_MONSTER:
		LOG(LogType::SendLog, std::format("AddMonster PKT to [{}]", id));
		break;
	case S_REMOVE_MONSTER:
		LOG(LogType::SendLog, std::format("RemoveMonster PKT to [{}]", id));
		break;
	case S_MONSTER_STATUS_UPDATE:
		LOG(LogType::SendLog, std::format("Monsterpdate PKT to [{}]", id));
		break;
	default:
		LOG(LogType::SendLog, "Unknown Packet Type");
		break;
	}
#pragma endregion
	auto send_data = new ExpOver{ reinterpret_cast<BYTE*>(packet) };
	WSASend(socket, &send_data->wsabuf, 1, nullptr, 0, &send_data->wsaover, nullptr);
}

void Session::Login()
{
	bLogin = true;
}

void Session::Connect(SOCKET& socket, int32 id)
{
	this->id = id;
	this->socket = socket;

	player = std::make_shared<Player>();
	player->Init();
	player->GetInfo().ID = id;
}

void Session::Disconnect()
{
	bLogin = false;
	closesocket(socket);
}
