#pragma once

struct DBSignUpResult
{
	DBResultCode code;
	uint32 dbId = 0;
};

struct DBLoginResult
{
	DBResultCode code;
	uint32 dbId = 0;
	std::string nickname;
	FInfoData info;
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
	DBSignUpResult SignUpUser(const std::string& login_id, const std::string& password, const std::wstring& nickname);
	DBLoginResult CheckLogin(const std::string& login_id, const std::string& password);
	bool UpdatePlayerInfo(uint32 dbId, const FInfoData& info);

private:
	std::shared_ptr<mysqlx::Session> _dbsess;
	std::shared_ptr<mysqlx::Schema> _db;

	const std::string USERS_TABLE = "users";
	mysqlx::Table GetUsersTable();
};