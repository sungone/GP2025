#pragma once

struct DBLoginResult
{
	DBResultCode code;
	uint32 dbId = 0;
	FInfoData info;
	std::vector<std::pair<uint32, uint8>> items;
	std::vector<FFriendInfo> friends;
};


class DBManager
{
public:
	static DBManager& GetInst() {
		static DBManager inst;
		return inst;
	}
	bool Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& schema);
	void Shutdown();
	DBLoginResult SignUpUser(int32 sessionId, const std::string& login_id, const std::string& password, const std::wstring& nickname);
	DBLoginResult CheckLogin(int32 sessionId, const std::string& login_id, const std::string& password);
	bool UpdatePlayerInfo(uint32 dbId, const FInfoData& info);
	bool AddUserItem(uint32 dbId, uint32 itemID, uint8 itemTypeID);
	bool RemoveUserItem(uint32 dbId, uint32 itemID);

	DBResultCode SendFriendRequest(uint32 fromId, uint32 toId);
	bool IsFriendOrPending(uint32 userId, uint32 targetId);
	std::pair<DBResultCode, std::optional<FFriendInfo>> AcceptFriendRequest(uint32 fromId, uint32 toId);
	DBResultCode RemoveFriendRequest(uint32 fromId, uint32 toId);
	DBResultCode RemoveFriend(uint32 userId, uint32 friendId);
	std::pair<DBResultCode, std::vector<FFriendInfo>> GetFriendList(uint32 userId);
	int32 FindUserDBId(const std::wstring& nickname);

};