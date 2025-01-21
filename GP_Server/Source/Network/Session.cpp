#include "Session.h"
#include "SessionManager.h"
#include "GameObjectManager.h"

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
	switch (packet->Header.PacketType)
	{
	case S_PLAYER_LOGIN_INFO:
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
	default:
		LOG(LogType::SendLog, "Unknown Packet Type");
		break;
	}
	auto send_data = new ExpOver{ reinterpret_cast<unsigned char*>(packet) };
	WSASend(socket, &send_data->wsabuf, 1, nullptr, 0, &send_data->wsaover, nullptr);
}

void Session::Login()
{
	bLogin = true;
	GameObjectManager::GetInst().SetBunkerRandomLocation(info);
}

void Session::Disconnect()
{
	bLogin = false;
	closesocket(socket);
}
