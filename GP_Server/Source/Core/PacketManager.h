#pragma once
#include "PlayerSession.h"
#include "SessionManager.h"
#include "GameWorldManager.h"

class PacketManager
{
public:
	static PacketManager& GetInst()
	{
		static PacketManager inst;
		return inst;
	}
	void ProcessPacket(int32 sessionId, Packet* packet);

private:
	void HandleSignUpPacket(int32 sessionId, Packet* packet);
	void HandleLoginPacket(int32 sessionId, Packet* packet);
	void HandleLogoutPacket(int32 sessionId);
	void HandleSelectCharacterPacket(int32 sessionId, Packet* packet);
	void HandleEnterGamePacket(int32 sessionId, Packet* packet);

	void HandleMovePacket(int32 sessionId, Packet* packet);
	void HandleStartAimingPacket(int32 sessionId, Packet* packet);
	void HandleStopAimingPacket(int32 sessionId, Packet* packet);
	void HandleAttackPacket(int32 sessionId, Packet* packet);
	void HandleUseSkillPacket(int32 sessionId, Packet* packet);
	void HandleEndSkillPacket(int32 sessionId, Packet* packet);
	void HandleRemoveStatePacket(int32 sessionId, Packet* packet);
	
	void HandlePickUpItemPacket(int32 sessionId, Packet* packet);
	void HandleDropItemPacket(int32 sessionId, Packet* packet);
	void HandleUseItemPacket(int32 sessionId, Packet* packet);
	void HandleEquipItemPacket(int32 sessionId, Packet* packet);
	void HandleUnequipItemPacket(int32 sessionId, Packet* packet);

	void HandleZoneChangeRequestPacket(int32 sessionId, Packet* packet);
	void HandleRespawnRequestPacket(int32 sessionId, Packet* packet);

	void HandleShopBuyItemPacket(int32 sessionId, Packet* packet);
	void HandleShopSellItemPacket(int32 sessionId, Packet* packet);

	void HandleRequestQuestPacket(int32 sessionId, Packet* packet);
	void HandleCompleteQuestPacket(int32 sessionId, Packet* packet);
	void HandleRejectQuestPacket(int32 sessionId, Packet* packet);

	void HandleChatSendPacket(int32 sessionId, Packet* packet);
	void HandleChatWhisperPacket(int32 sessionId, Packet* packet);
	
	void HandleFriendAddRequestPacket(int32 sessionId, Packet* packet);
	void HandleFriendAcceptRequestPacket(int32 sessionId, Packet* packet);
	void HandleFriendRemoveRequestPacket(int32 sessionId, Packet* packet);
	void HandleFriendRejectRequestPacket(int32 sessionId, Packet* packet);
	void HandleChangeChannelPacket(int32 sessionId, Packet* packet);
private:
	GameWorld* GetValidWorld(int32 sessionId);
	template<typename TPacket, typename Func>
	void HandleWithWorld(int32 sessionId, Packet* packet, Func&& func)
	{
		auto* p = static_cast<TPacket*>(packet);
		if (auto* world = GetValidWorld(sessionId))
			func(world, p);
	}
private:
	SessionManager& _sessionMgr = SessionManager::GetInst();
	DBManager& _dbMgr = DBManager::GetInst();
};

