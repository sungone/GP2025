#pragma once
#include "PlayerSession.h"
#include "SessionManager.h"
#include "GameWorld.h"

class PacketManager
{
public:
	static PacketManager& GetInst()
	{
		static PacketManager inst;
		return inst;
	}

	void ProcessPacket(int32 sessionId, Packet* packet);

	void HandleSignUpPacket(int32 sessionId, Packet* packet);
	void HandleLoginPacket(int32 sessionId, Packet* packet);
	void HandleLogoutPacket(int32 sessionId);
	void HandleMovePacket(int32 sessionId, Packet* packet);
	void HandleAttackPacket(int32 sessionId, Packet* packet);
	void HandleUseSkillPacket(int32 sessionId, Packet* packet);
	
	void HandlePickUpItemPacket(int32 sessionId, Packet* packet);
	void HandleDropItemPacket(int32 sessionId, Packet* packet);
	void HandleUseItemPacket(int32 sessionId, Packet* packet);
	void HandleEquipItemPacket(int32 sessionId, Packet* packet);
	void HandleUnequipItemPacket(int32 sessionId, Packet* packet);

private:
	SessionManager& _sessionMgr = SessionManager::GetInst();
	DBManager& _dbMgr = DBManager::GetInst();
	GameWorld& _gameWorld = GameWorld::GetInst();
};

