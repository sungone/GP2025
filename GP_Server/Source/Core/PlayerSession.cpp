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
	case S_SIGNUP_SUCCESS:
		LOG(LogType::SendLog, std::format("SignUpSuccess PKT to [{}]", _id));
		break;
	case S_SIGNUP_FAIL:
		LOG(LogType::SendLog, std::format("SignUpFail PKT to [{}]", _id));
		break;
	case S_LOGIN_SUCCESS:
		LOG(LogType::SendLog, std::format("LoginSuccess PKT to [{}]", _id));
		break;
	case S_LOGIN_FAIL:
		LOG(LogType::SendLog, std::format("LoginFail PKT to [{}]", _id));
		break;
	case S_ENTER_GAME:
		LOG(LogType::SendLog, std::format("EnterGame PKT to [{}]", _id));
		break;

	case S_ADD_PLAYER:
		LOG(LogType::SendLog, std::format("AddPlayer PKT to [{}]", _id));
		break;
	case S_REMOVE_PLAYER:
		LOG(LogType::SendLog, std::format("RemovePlayer PKT to [{}]", _id));
		break;
	case S_PLAYER_MOVE:
		LOG(LogType::SendLog, std::format("PlayerMove PKT to [{}]", _id));
		break;
	case S_PLAYER_STATUS_UPDATE:
		LOG(LogType::SendLog, std::format("PlayerUpdate PKT to [{}]", _id));
		break;
	case S_DAMAGED_PLAYER:
		LOG(LogType::SendLog, std::format("DamagedPlayer PKT to [{}]", _id));
		break;
	case S_PLAYER_USE_SKILL:
		LOG(LogType::SendLog, std::format("PlayerUseSkill PKT to [{}]", _id));
		break;
	case S_SKILL_UNLOCK:
		LOG(LogType::SendLog, std::format("SkillUnlock PKT to [{}]", _id));
		break;
	case S_SKILL_UPGRADE:
		LOG(LogType::SendLog, std::format("SkillUpgrade PKT to [{}]", _id));
		break;
	case S_LEVEL_UP:
		LOG(LogType::SendLog, std::format("LevelUp PKT to [{}]", _id));
		break;

	case S_ADD_MONSTER:
		LOG(LogType::SendLog, std::format("AddMonster PKT to [{}]", _id));
		break;
	case S_REMOVE_MONSTER:
		LOG(LogType::SendLog, std::format("RemoveMonster PKT to [{}]", _id));
		break;
	case S_MONSTER_STATUS_UPDATE:
		LOG(LogType::SendLog, std::format("MonsterUpdate PKT to [{}]", _id));
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
	case S_ADD_INVENTORY_ITEM:
		LOG(LogType::SendLog, std::format("AddInventory PKT to [{}]", _id));
		break;
	case S_USE_INVENTORY_ITEM:
		LOG(LogType::SendLog, std::format("RemoveInventory PKT to [{}]", _id));
		break;
	case S_EQUIP_ITEM:
		LOG(LogType::SendLog, std::format("EquipItem PKT to [{}]", _id));
		break;
	case S_UNEQUIP_ITEM:
		LOG(LogType::SendLog, std::format("UnequipItem PKT to [{}]", _id));
		break;

	case S_CHANGE_ZONE:
		LOG(LogType::SendLog, std::format("ChangeZone PKT to [{}]", _id));
		break;
	case S_RESPAWN:
		LOG(LogType::SendLog, std::format("Respawn PKT to [{}]", _id));
		break;

	case S_SHOP_ITEM_LIST:
		LOG(LogType::SendLog, std::format("ShopItemList PKT to [{}]", _id));
		break;
	case S_SHOP_BUY_RESULT:
		LOG(LogType::SendLog, std::format("ShopBuyResult PKT to [{}]", _id));
		break;
	case S_SHOP_SELL_RESULT:
		LOG(LogType::SendLog, std::format("ShopSellResult PKT to [{}]", _id));
		break;
	case S_QUEST_REWARD:
		LOG(LogType::SendLog, std::format("QuestReward PKT to [{}]", _id));
		break;

	case S_CHAT_BROADCAST:
		LOG(LogType::SendLog, std::format("ChatBroadcast PKT to [{}]", _id));
		break;

	default:
		LOG(LogType::SendLog, "Unknown Packet Type");
		break;
	}
#pragma endregion
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