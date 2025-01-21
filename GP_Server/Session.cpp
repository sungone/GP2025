#include "Session.h"
#include "SessionManager.h"

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
	case S_LOGININFO:
		LOG(LogType::SendLog, std::format("LoginInfo PKT to [{}]", id));
		break;
	case S_ADD_PLAYER:
		LOG(LogType::SendLog, std::format("AddPlayer PKT to [{}]", id));
		break;
	case S_REMOVE_PLAYER:
		LOG(LogType::SendLog, std::format("RemovePlayer PKT to [{}]", id));
		break;
	case S_MOVE_PLAYER:
		LOG(LogType::SendLog, std::format("MovePlayer PKT to [{}]", id));
		break;
	case S_ATTACK_PLAYER:
		LOG(LogType::SendLog, std::format("AttackPlayer PKT to [{}]", id));
		break;
	default:
		break;
	}
	auto send_data = new ExpOver{ reinterpret_cast<unsigned char*>(packet) };
	WSASend(socket, &send_data->wsabuf, 1, nullptr, 0, &send_data->wsaover, nullptr);
}

void Session::Login()
{
	static std::default_random_engine dre;
	static std::uniform_real_distribution<float> ud_x(-3000, -1000);
	static std::uniform_real_distribution<float> ud_y(-3500, -1500);

	bLogin = true;
	info.X = ud_x(dre);
	info.Y = ud_y(dre);
	info.Z = 116;
}

void Session::Disconnect()
{
	bLogin = false;
	closesocket(socket);
}
