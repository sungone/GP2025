#pragma once
#include "Session.h"
#include "SessionManager.h"
#include "GameManager.h"

class PacketManager
{
public:
	static PacketManager& GetInst()
	{
		static PacketManager inst;
		return inst;
	}

	void ProcessPacket(Session& session, char* packet);
	void HandleLoginPacket(Session& session);
	void HandleLogoutPacket(Session& session);
	void HandleMovePacket(Session& session, char* packet);
	void HandleAttackPacket(Session& session, char* packet);

private:
	std::array<Session, MAX_PLAYER>& sessions = SessionManager::GetInst().GetSessions();
	SessionManager& sessionMgr = SessionManager::GetInst();
	GameManager& gameMgr = GameManager::GetInst();
};

