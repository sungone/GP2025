#include "pch.h"
#include "PacketManager.h"

void PacketManager::ProcessPacket(Session& session, BYTE* packet)
{
	EPacketType packetType = static_cast<EPacketType>(packet[PKT_TYPE_INDEX]);

	switch (packetType)
	{
	case EPacketType::C_LOGIN:
		HandleLoginPacket(session);
		break;
	case EPacketType::C_LOGOUT:
		HandleLogoutPacket(session);
		break;
	case EPacketType::C_MOVE:
		HandleMovePacket(session, packet);
		break;
	case EPacketType::C_ATTACK:
		HandleAttackPacket(session, packet);
		break;
	case EPacketType::C_TAKE_ITEM:
		HandleTakeItemPacket(session, packet);
		break;
	case EPacketType::C_DROP_ITEM:
		HandleDropItemPacket(session, packet);
		break;
	case EPacketType::C_USE_ITEM:
		HandleUseItemPacket(session, packet);
		break;
	case EPacketType::C_EQUIP_ITEM:
		HandleEquipItemPacket(session, packet);
		break;
	case EPacketType::C_UNEQUIP_ITEM:
		HandleUnequipItemPacket(session, packet);
		break;
	default:
		LOG(LogType::RecvLog, "Unknown Packet Type");
	}
}

void PacketManager::HandleLoginPacket(Session& session)
{
	session.CreatePlayer();

	auto& playerInfo = session.GetPlayerInfo();
	int32 id = playerInfo.ID;
	LOG(LogType::RecvLog, std::format("Login PKT [{}]", id));

	auto loginPkt = InfoPacket(EPacketType::S_LOGIN_SUCCESS, playerInfo);
	session.DoSend(&loginPkt);

	auto myInfoPkt = InfoPacket(EPacketType::S_ADD_PLAYER, playerInfo);
	_sessionMgr.Broadcast(&myInfoPkt, id);
	_sessionMgr.HandleLogin(id);
	_gameMgr.SpawnMonster(session);
}

void PacketManager::HandleLogoutPacket(Session& session)
{
	int32 id = session.GetId();
	LOG(LogType::RecvLog, std::format("Logout PKT [{}]", id));
	
	auto pkt = IDPacket(EPacketType::S_REMOVE_PLAYER, id);
	_sessionMgr.Broadcast(&pkt, id);
	_sessionMgr.Disconnect(id);
}

void PacketManager::HandleMovePacket(Session& session, BYTE* packet)
{
	auto& playerInfo = session.GetPlayerInfo();
	int32 id = playerInfo.ID;
	LOG(LogType::RecvLog, std::format("Move PKT [{}] ({:.2f}, {:.2f}, {:.2f} / Yaw: {:.2f}, State {})",
		playerInfo.ID, playerInfo.Pos.X, playerInfo.Pos.Y, playerInfo.Pos.Z, playerInfo.Yaw, playerInfo.State));

	InfoPacket* p = reinterpret_cast<InfoPacket*>(packet);
	playerInfo = p->Data;
	auto pkt = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, playerInfo);
	_sessionMgr.Broadcast(&pkt, id);
}

void PacketManager::HandleAttackPacket(Session& session, BYTE* packet)
{
	auto& playerInfo = session.GetPlayerInfo();
	int32 id = playerInfo.ID;
	LOG(LogType::RecvLog, std::format("Attack PKT [{}]", id));
	AttackPacket* p = reinterpret_cast<AttackPacket*>(packet);
	_gameMgr.ProcessAttack(id, p->TargetID);
	playerInfo.AddState(ECharacterStateType::STATE_AUTOATTACK);
	auto pkt1 = InfoPacket(EPacketType::S_PLAYER_STATUS_UPDATE, playerInfo);
	_sessionMgr.Broadcast(&pkt1);

}

void PacketManager::HandleTakeItemPacket(Session& session, BYTE* packet)
{
	IDPacket* p = reinterpret_cast<IDPacket*>(packet);
	auto itemid = p->Data;
	//Todo: 아이템아이디로 아이템컨테이너에서 찾아 플레이어 인벤토리에 추가
	_gameMgr.PickUpItem(session.GetId() , itemid);
	auto pkt1 = ItemPkt::PickUpPacket(p->Data);
	_sessionMgr.Broadcast(&pkt1);
}

void PacketManager::HandleDropItemPacket(Session& session, BYTE* packet)
{
	IDPacket* p = reinterpret_cast<IDPacket*>(packet);
	//Todo: 아이템아이디로 플레이어 인벤토리에서 꺼내서 월드에 스폰
}

void PacketManager::HandleUseItemPacket(Session& session, BYTE* packet)
{
	IDPacket* p = reinterpret_cast<IDPacket*>(packet);
	//Todo: 아이템아이디로 플레이어 인벤토리에서 찾아
	// 타입(EUseable type)에 따른 값 스텟에 적용
}

void PacketManager::HandleEquipItemPacket(Session& session, BYTE* packet)
{
	IDPacket* p = reinterpret_cast<IDPacket*>(packet);
	//Todo: 다른플레이어에게 착용한 아이템타입 전송
}

void PacketManager::HandleUnequipItemPacket(Session& session, BYTE* packet)
{
	IDPacket* p = reinterpret_cast<IDPacket*>(packet);
	//Todo: 다른플레이어에게 착용한 아이템타입 전송
	// -> 근데 착용 아이템 교체일텐데 unequip도 필요할까나..?
	//  아무것도 착용 안할 때만 보내는 거로 해야하나

}