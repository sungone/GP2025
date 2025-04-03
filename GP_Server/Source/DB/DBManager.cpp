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
	catch (const mysqlx::Error& e) {
		LOG(LogType::Error, std::format("{}", e.what()));
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

void DBManager::PrintUsers()
{
	try {
		mysqlx::Table users = _db->getTable("users");
		auto rows = users.select("login_id", "nickname").execute();
		for (auto row : rows) {
			std::cout << "login_id: " << row[0] << ", nickname: " << row[1] << '\n';
		}
	}
	catch (const mysqlx::Error& e) {
		LOG(LogType::Error, std::format("{}", e.what()));
	}
}
