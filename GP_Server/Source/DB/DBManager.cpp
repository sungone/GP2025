#include "pch.h"
#include "DBManager.h"

bool DBManager::Connect(const std::string& host, int port,
	const std::string& user, const std::string& pass, const std::string& schemaName)
{
	try {
		_dbsess = std::make_shared<mysqlx::Session>(host, port, user, pass);
		_db = std::make_shared<mysqlx::Schema>(_dbsess->getSchema(schemaName));
		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG(LogType::Error, std::format("MySQL Error: {}", e.what()));
		return false;
	}
}

void DBManager::Close()
{
	_db.reset();
	if (_dbsess) {
		_dbsess->close();
		_dbsess.reset();
	}
}

void DBManager::PrintUsersTable()
{
	try {
		auto users = GetUsersTable();
		auto rows = users.select("id", "login_id", "nickname").execute();
		std::cout << "\n============ User Table ============\n";
		for (auto row : rows)
		{
			int uid = row[0].get<int>();
			std::string loginId = row[1].get<std::string>();
			std::string nickname = row[2].get<std::string>();

			std::cout << std::format("UID: {:<4} LoginID: {:<12} - {}\n", uid, loginId, nickname);
		}
		std::cout << '\n';
	}
	catch (const mysqlx::Error& e)
	{
		LOG(LogType::Error, std::format("MySQL Error: {}", e.what()));
	}
}

DBSignUpResult DBManager::SignUpUser(const std::string& login_id, const std::string& password, const std::wstring& nickname)
{
	try {
		auto result = GetUsersTable()
			.insert("login_id", "password", "nickname")
			.values(login_id, password, nickname)
			.execute();

		uint32 userId = static_cast<uint32>(result.getAutoIncrementValue());
		if (isPrint) PrintUsersTable();
		return { DBResultCode::SUCCESS, userId };
	}
	catch (const mysqlx::Error& e)
	{
		std::string msg = e.what();
		if (msg.find("Duplicate entry") != std::string::npos)
		{
			LOG(LogType::Warning, "be duplicated");
			return { DBResultCode::DUPLICATE_ID };
		}

		LOG(LogType::Error, std::format("MySQL Error: {}", msg));
		return { DBResultCode::DB_ERROR };
	}
}

DBLoginResult DBManager::CheckLogin(const std::string& login_id, const std::string& password)
{
	try {
		auto result = GetUsersTable()
			.select("id", "password", "nickname")
			.where("login_id = :login_id")
			.bind("login_id", login_id)
			.execute();

		auto row = result.fetchOne();
		if (!row)
			return { DBResultCode::INVALID_USER };

		std::string dbPassword = row[1].get<std::string>();
		if (dbPassword != password)
			return { DBResultCode::INVALID_PASSWORD };

		uint32 userId = static_cast<uint32>(row[0].get<int>());
		std::string nickname = row[2].get<std::string>();

		if (isPrint) PrintUsersTable();
		return { DBResultCode::SUCCESS, userId, nickname };
	}
	catch (const mysqlx::Error& e)
	{
		LOG(LogType::Error, std::format("MySQL Error (Login): {}", e.what()));
		return { DBResultCode::DB_ERROR };
	}
}


mysqlx::Table DBManager::GetUsersTable()
{
	return _db->getTable(USERS_TABLE);
}