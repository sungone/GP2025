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

	void ProcessPacket(Session& session, BYTE* packet);
	void HandleLoginPacket(Session& session);
	void HandleLogoutPacket(Session& session);
	void HandleMovePacket(Session& session, BYTE* packet);
	void HandleAttackPacket(Session& session, BYTE* packet);

private:
	std::array<Session, MAX_CLIENT>& sessions = SessionManager::GetInst().GetSessions();
	SessionManager& sessionMgr = SessionManager::GetInst();
	GameManager& gameMgr = GameManager::GetInst();
};

