#pragma once
#include "Define.h"
#include "Session.h"
#include "IOCP.h"

class SessionManager
{
public:
	static SessionManager& GetInst()
	{
		static SessionManager inst;
		return inst;
	}

	void RegisterSession(SOCKET& socket);
	void Disconnect(int id);

	void DoRecv(int id);
	void HandleRecvBuffer(int id, int recvByte, ExpOver* expOver);

	void SendInfoPacket(EPacketType type, Session& destSession, int sourceId)
	{
		static const std::unordered_map<EPacketType, std::string> packetTypeNames = {
			{ S_LOGININFO, "S_LOGININFO" },
			{ S_ADD_PLAYER, "S_ADD_PLAYER" },
			{ S_REMOVE_PLAYER, "S_REMOVE_PLAYER" },
			{ S_MOVE_PLAYER, "S_MOVE_PLAYER" },
			{ S_ATTACK_PLAYER, "S_ATTACK_PLAYER" }
		};

		auto pk = FObjectInfoPacket(type, clients[sourceId].info);

		const auto it = packetTypeNames.find(type);
		const std::string& typeName = (it != packetTypeNames.end()) ? it->second : "UNKNOWN_TYPE";

		LOG(LogType::SendLog, std::format("{} PKT To [{}]", typeName, destSession.getId()));
		destSession.DoSend(&pk);
	}

private:
	int32 GenerateId()
	{
		for (int i = 1; i < MAX_CLIENT + 1; ++i)
		{
			if (clients[i].bLogin) continue;
			return i;
		}
	}

public:
	std::array<Session, MAX_CLIENT> clients;
private:
	IOCP& iocp = IOCP::GetInst();
};

