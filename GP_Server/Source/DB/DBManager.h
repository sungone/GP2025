#pragma once

struct DBLoginResult
{
	DBResultCode code;
	uint32 dbId = 0;
	FInfoData info;
	std::vector<std::pair<uint32, uint8>> items;
};

class DBManager
{
public:
	static DBManager& GetInst() {
		static DBManager inst;
		return inst;
	}
	bool Connect(const std::string& host, const std::string& user, const std::string& pass, const std::string& schema);
	void Close();
	DBLoginResult SignUpUser(int32 sessionId, const std::string& login_id, const std::string& password, const std::wstring& nickname);
	DBLoginResult CheckLogin(int32 sessionId, const std::string& login_id, const std::string& password);
	bool UpdatePlayerInfo(uint32 dbId, const FInfoData& info);
	bool AddUserItem(uint32 dbId, uint32 itemID, uint8 itemTypeID);
	bool RemoveUserItem(uint32 dbId, uint32 itemID);
};