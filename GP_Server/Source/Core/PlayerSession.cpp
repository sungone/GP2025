#include "pch.h"
#include "PlayerSession.h"
#include "SessionManager.h"
#include "GameWorld.h"
#include "PacketManager.h"
#include "magic_enum/magic_enum.hpp"
void PlayerSession::DoRecv()
{
	_sSocket->DoRecv();
}

void PlayerSession::DoSend(const Packet* packet)
{
	auto name = std::string(magic_enum::enum_name(static_cast<EPacketType>(packet->Header.PacketType)));
	if (name.empty()) name = "Unknown";
	LOG(LogType::SendLog, std::format("{} PKT to [{}]", name, _id));

	_sSocket->DoSend(packet);
}

void PlayerSession::HandleRecvBuffer(int32 recvByte, ExpOver* expOver)
{
	_sSocket->HandleRecvBuffer(_id, recvByte, expOver);
}

void PlayerSession::Connect(SOCKET socket, int32 id)
{
	this->_id = id;
	_sSocket = std::make_unique<SessionSocket>(socket);
}

void PlayerSession::Disconnect()
{
	Logout();
	_sSocket->Close();
}

void PlayerSession::Login(const DBLoginResult& dbRes)
{
	_state = SessionState::LoggedIn;
	_player = std::make_shared<Player>(_id);
#ifdef DB_LOCAL
	{
		_dbId = dbRes.dbId;
		_player->LoadFromDB(dbRes);
	}
#endif
}

void PlayerSession::EnterGame()
{
	_state = SessionState::InGame;
}

void PlayerSession::Logout()
{
	if(IsLogin())
	{
		_state = SessionState::None;
#ifdef DB_LOCAL
		_player->SaveToDB(_dbId);
#endif
	}
}

int32 PlayerSession::GetId()
{
	return _id;
}

FInfoData& PlayerSession::GetPlayerInfo()
{
	return _player->GetInfo();
}

std::shared_ptr<Player> PlayerSession::GetPlayer()
{
	return _player;
}