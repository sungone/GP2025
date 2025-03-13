#pragma once
#include "Session.h"
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

	void ProcessPacket(int32 sessionId, BYTE* packet);

	void HandleLoginPacket(int32 sessionId);
	void HandleLogoutPacket(int32 sessionId);
	void HandleMovePacket(int32 sessionId, BYTE* packet);
	void HandleAttackPacket(int32 sessionId, BYTE* packet);

	void HandlePickUpItemPacket(int32 sessionId, BYTE* packet);
	void HandleDropItemPacket(int32 sessionId, BYTE* packet);
	void HandleUseItemPacket(int32 sessionId, BYTE* packet);
	void HandleEquipItemPacket(int32 sessionId, BYTE* packet);
	void HandleUnequipItemPacket(int32 sessionId, BYTE* packet);

private:
	SessionManager& _sessionMgr = SessionManager::GetInst();
	GameWorld& _gameWorld = GameWorld::GetInst();
};

