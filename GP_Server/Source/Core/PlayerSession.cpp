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
	LOG_D("{} PKT to [{}]", name, _id);

	_sSocket->DoSend(packet);
}

void PlayerSession::OnRecv(int32 recvByte, ExpOver* expOver)
{
	_sSocket->OnRecv(_id, recvByte, expOver);
}

void PlayerSession::Connect(SOCKET socket, int32 id)
{
	this->_id = id;
	_sSocket = std::make_unique<SessionSocket>(socket);
}

void PlayerSession::Disconnect()
{
	Logout();
	_sSocket->Disconnect();
	_sSocket.reset();
}

void PlayerSession::Login(const DBLoginResult& dbRes)
{
	_state = SessionState::LoggedIn;
	_player = std::make_shared<Player>(_id);
#ifdef DB_MODE
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
		//todo: 뷰리스트 제거
		_state = SessionState::None;
#ifdef DB_MODE
		_player->SaveToDB(_dbId);
		_player.reset();
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