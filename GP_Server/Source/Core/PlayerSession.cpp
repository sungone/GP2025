#include "pch.h"
#include "PlayerSession.h"
#include "SessionManager.h"
#include "GameWorld.h"
#include "PacketManager.h"

void PlayerSession::DoRecv()
{
	_sSocket->DoRecv();
}
void PlayerSession::DoSend(const Packet* packet)
{
#pragma region //Log
	switch (packet->Header.PacketType)
	{
	case S_LOGIN_SUCCESS:
		LOG(LogType::SendLog, std::format("LoginInfo PKT to [{}]", _id));
		break;
	case S_ADD_PLAYER:
		//LOG(LogType::SendLog, std::format("AddPlayer PKT to [{}]", _id));
		break;
	case S_REMOVE_PLAYER:
		LOG(LogType::SendLog, std::format("RemovePlayer PKT to [{}]", _id));
		break;
	case S_PLAYER_STATUS_UPDATE:
		LOG(LogType::SendLog, std::format("PlayerUpdate PKT to [{}]", _id));
		break;
	case S_DAMAGED_PLAYER:
		LOG(LogType::SendLog, std::format("DamagedPlayer PKT to [{}]", _id));
		break;
	case S_ADD_MONSTER:
		//LOG(LogType::SendLog, std::format("AddMonster PKT to [{}]", _id));
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
	case S_ITEM_SPAWN:
		LOG(LogType::SendLog, std::format("ItemSpawn PKT to [{}]", _id));
		break;
	case S_ITEM_DESPAWN:
		LOG(LogType::SendLog, std::format("ItemDespawn PKT to [{}]", _id));
		break;
	case S_ITEM_PICKUP:
		LOG(LogType::SendLog, std::format("ItemPickUp PKT to [{}]", _id));
		break;
	case S_ITEM_DROP:
		LOG(LogType::SendLog, std::format("ItemDrop PKT to [{}]", _id));
		break;
	case S_ADD_IVENTORY_ITEM:
		LOG(LogType::SendLog, std::format("AddInventory PKT to [{}]", _id));
		break;
	case S_REMOVE_IVENTORY_ITEM:
		LOG(LogType::SendLog, std::format("RemoveInventory PKT to [{}]", _id));
		break;
	case S_EQUIP_ITEM:
		LOG(LogType::SendLog, std::format("EquipItem PKT to [{}]", _id));
		break;
	case S_UNEQUIP_ITEM:
		LOG(LogType::SendLog, std::format("UnequipItem PKT to [{}]", _id));
		break;
	default:
		LOG(LogType::SendLog, "Unknown Packet Type");
		break;
	}
#pragma endregion
	_sSocket->DoSend(packet);
}

void PlayerSession::Connect(SOCKET socket, int32 id)
{
	this->_id = id;
	_sSocket = std::make_unique<SessionSocket>(socket);
}

void PlayerSession::Disconnect()
{
	_loginState = false;
	_sSocket->Close();
}

void PlayerSession::Login()
{
	_loginState = true;
	CreatePlayer();
}

void PlayerSession::CreatePlayer()
{
	_player = std::make_shared<Player>();
	_player->Init();
	_player->GetInfo().ID = _id;
	GameWorld::GetInst().AddPlayer(_player);
}

int32 PlayerSession::GetId()
{
	return _id;
}

FInfoData& PlayerSession::GetPlayerInfo()
{
	return _player->GetInfo();
}

void PlayerSession::HandleRecvBuffer(int32 recvByte, ExpOver* expOver)
{
	_sSocket->HandleRecvBuffer(_id, recvByte, expOver);
}