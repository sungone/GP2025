#include "pch.h"
#include "DBManager.h"

bool DBManager::Connect(const std::string& host, const std::string& user, const std::string& pwd, const  std::string& db)
{
	try {
		_dbsess = std::make_shared<mysqlx::Session>(mysqlx::SessionSettings(host, user, pwd, db));
		_db = std::make_shared<mysqlx::Schema>(_dbsess->getSchema(db));
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
		PRINT("============ User Table ============");
		for (auto row : rows)
		{
			int uid = row[0].get<int>();
			std::string loginId = row[1].get<std::string>();
			std::string nickname = row[2].get<std::string>();
			PRINT(std::format("UID: {:<4} LoginID: {:<12} - {}\n", uid, loginId, nickname));
		}
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

		uint32 dbId = static_cast<uint32>(result.getAutoIncrementValue());
		if (isPrint) PrintUsersTable();
		return { DBResultCode::SUCCESS, dbId };
	}
	catch (const mysqlx::Error& e)
	{
		std::string msg = e.what();
		if (msg.find("Duplicate entry") != std::string::npos)
		{
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

bool DBManager::CreatePlayerInfo(uint32 dbId, const FInfoData& info)
{
	try {
		auto table = _db->getTable("player_info");
		table.insert("id", "nickname", "character_type",
			"pos_x", "pos_y", "pos_z", "yaw",
			"level", "exp", "max_exp", "hp", "max_hp",
			"damage", "crt_rate", "crt_value", "dodge", "speed",
			"skill_level", "gold")
			.values(dbId, info.GetName(), info.CharacterType,
				info.Pos.X, info.Pos.Y, info.Pos.Z, info.Yaw,
				info.Stats.Level, info.Stats.Exp, info.Stats.MaxExp,
				info.Stats.Hp, info.Stats.MaxHp, info.Stats.Damage,
				info.Stats.CrtRate, info.Stats.CrtValue,
				info.Stats.Dodge, info.Stats.Speed,
				info.Skilllevel, info.Gold)
			.execute();

		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG(LogType::Error, std::format("MySQL Error (CreatePlayerInfo): {}", e.what()));
		return false;
	}
}

DBCharacterData DBManager::LoadPlayerInfo(uint32 dbId)
{
	FInfoData info;

	try {
		auto table = _db->getTable("player_info");
		auto result = table.select("id", "nickname", "character_type",
			"pos_x", "pos_y", "pos_z", "yaw",
			"level", "exp", "max_exp", "hp", "max_hp",
			"damage", "crt_rate", "crt_value", "dodge", "speed",
			"skill_level", "gold")
			.where("id = :id")
			.bind("id", dbId)
			.execute();

		auto row = result.fetchOne();
		if (!row)
			return { DBResultCode::INVALID_USER };

		info.ID = row[0].get<int>();
		strncpy_s(info.NickName, row[1].get<std::string>().c_str(), NICKNAME_LEN - 1);
		info.CharacterType = row[2].get<uint8>();
		info.Pos = FVector(row[3].get<float>(), row[4].get<float>(), row[5].get<float>());
		info.Yaw = row[6].get<float>();

		info.Stats.Level = row[7].get<uint32>();
		info.Stats.Exp = row[8].get<float>();
		info.Stats.MaxExp = row[9].get<float>();
		info.Stats.Hp = row[10].get<float>();
		info.Stats.MaxHp = row[11].get<float>();
		info.Stats.Damage = row[12].get<float>();
		info.Stats.CrtRate = row[13].get<float>();
		info.Stats.CrtValue = row[14].get<float>();
		info.Stats.Dodge = row[15].get<float>();
		info.Stats.Speed = row[16].get<float>();

		info.Skilllevel = row[17].get<uint32>();
		info.Gold = row[18].get<uint32>();

		return { DBResultCode::SUCCESS, info };
	}
	catch (const mysqlx::Error& e)
	{
		LOG(LogType::Error, std::format("MySQL Error (LoadPlayerInfo): {}", e.what()));
		return { DBResultCode::DB_ERROR };
	}
}



mysqlx::Table DBManager::GetUsersTable()
{
	return _db->getTable(USERS_TABLE);
}