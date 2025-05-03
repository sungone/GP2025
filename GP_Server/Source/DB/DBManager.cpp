#include "pch.h"
#include "DBManager.h"
#include "GameWorld.h"

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

DBLoginResult DBManager::SignUpUser(int32 sessionId, const std::string& login_id, const std::string& password, const std::wstring& nickname)
{
	FInfoData newinfo;
	newinfo.ID = sessionId;
	newinfo.SetName(nickname);
	newinfo.CharacterType = static_cast<uint8>(Type::EPlayer::WARRIOR);

	if (newinfo.CharacterType == static_cast<uint8>(Type::EPlayer::WARRIOR)) {
		newinfo.fovAngle = 90;
		newinfo.AttackRadius = 300;
	}
	else {
		newinfo.fovAngle = 10;
		newinfo.AttackRadius = 1500;
	}

	uint32 level = newinfo.Stats.Level = 1;
	newinfo.Stats.Speed = 200.f;
	newinfo.CollisionRadius = 50.f;

	const FStatData* newStats = PlayerLevelTable::GetInst().GetStatByLevel(level);
	if (!newStats) {
		LOG(Warning, "Invalid level stat");
		return { DBResultCode::DB_ERROR };
	}

	auto& stats = newinfo.Stats;
	stats.MaxHp = newStats->MaxHp;
	stats.Hp = stats.MaxHp;
	stats.Damage = newStats->Damage;
	stats.CrtRate = newStats->CrtRate;
	stats.CrtValue = newStats->CrtValue;
	stats.Dodge = newStats->Dodge;
	stats.MaxExp = newStats->MaxExp;

	FVector newPos{};
	do {
		newPos = Map::GetInst().GetRandomPos(ZoneType::TUK);
	} while (GameWorld::GetInst().IsCollisionDetected(newinfo));

	newinfo.SetLocation(newPos);

	try {
		auto result = GetUsersTable()
			.insert("login_id", "password", "nickname")
			.values(login_id, password, nickname)
			.execute();

		uint32 dbId = static_cast<uint32>(result.getAutoIncrementValue());

		_db->getTable("player_info")
			.insert("id", "character_type", "pos_x", "pos_y", "pos_z", "yaw",
				"collision_radius", "attack_radius", "fov_angle",
				"level", "exp", "max_exp", "hp", "max_hp", "damage",
				"crt_rate", "crt_value", "dodge", "speed", "gold",
				"skill1_gid", "skill1_level",
				"skill2_gid", "skill2_level",
				"skill3_gid", "skill3_level")
			.values(dbId, newinfo.CharacterType, newPos.X, newPos.Y, newPos.Z, newinfo.Yaw,
				newinfo.CollisionRadius, newinfo.AttackRadius, newinfo.fovAngle,
				stats.Level, stats.Exp, stats.MaxExp, stats.Hp, stats.MaxHp, stats.Damage,
				stats.CrtRate, stats.CrtValue, stats.Dodge, stats.Speed, newinfo.Gold,
				static_cast<int>(newinfo.Skills.Q.SkillGID), newinfo.Skills.Q.SkillLevel,
				static_cast<int>(newinfo.Skills.E.SkillGID), newinfo.Skills.E.SkillLevel,
				static_cast<int>(newinfo.Skills.R.SkillGID), newinfo.Skills.R.SkillLevel)
			.execute();


		return { DBResultCode::SUCCESS, dbId, newinfo };
	}
	catch (const mysqlx::Error& e)
	{
		std::string msg = e.what();
		if (msg.find("Duplicate entry") != std::string::npos) {
			return { DBResultCode::DUPLICATE_ID };
		}
		LOG(LogType::Error, std::format("MySQL Error: {}", msg));
		return { DBResultCode::DB_ERROR };
	}
}

DBLoginResult DBManager::CheckLogin(int32 sessionId, const std::string& login_id, const std::string& password)
{
	try {
		auto result = _dbsess->sql(
			"SELECT "
			"u.id, u.password, u.nickname, "
			"p.character_type, p.pos_x, p.pos_y, p.pos_z, p.yaw, "
			"p.collision_radius, p.attack_radius, p.fov_angle, "
			"p.level, p.exp, p.max_exp, p.hp, p.max_hp, "
			"p.damage, p.crt_rate, p.crt_value, p.dodge, p.speed, "
			"p.gold, "
			"p.skill1_gid, p.skill1_level, "
			"p.skill2_gid, p.skill2_level, "
			"p.skill3_gid, p.skill3_level "
			"FROM users u JOIN player_info p ON u.id = p.id WHERE u.login_id = ?"
		).bind(login_id).execute();


		auto row = result.fetchOne();
		if (!row)
			return { DBResultCode::INVALID_USER };

		std::string dbPassword = row[1].get<std::string>();
		if (dbPassword != password)
			return { DBResultCode::INVALID_PASSWORD };

		uint32 dbId = static_cast<uint32>(row[0].get<int>());
		std::string nickname = row[2].get<std::string>();

		FInfoData info;
		info.ID = sessionId;
		info.SetName(ConvertToWString(nickname));
		info.CharacterType = static_cast<uint8>(row[3].get<int>());
		info.Pos = FVector(row[4].get<float>(), row[5].get<float>(), row[6].get<float>());
		info.Yaw = row[7].get<float>();
		info.CollisionRadius = row[8].get<float>();
		info.AttackRadius = row[9].get<float>();
		info.fovAngle = row[10].get<float>();
		info.Stats.Level = static_cast<uint32>(row[11].get<int>());
		info.Stats.Exp = row[12].get<float>();
		info.Stats.MaxExp = row[13].get<float>();
		info.Stats.Hp = row[14].get<float>();
		info.Stats.MaxHp = row[15].get<float>();
		info.Stats.Damage = row[16].get<float>();
		info.Stats.CrtRate = row[17].get<float>();
		info.Stats.CrtValue = row[18].get<float>();
		info.Stats.Dodge = row[19].get<float>();
		info.Stats.Speed = row[20].get<float>();
		info.Gold = static_cast<uint32>(row[21].get<int>());
		info.Skills.Q = FSkillData((ESkillGroup)row[22].get<int>(), row[23].get<int>());
		info.Skills.E = FSkillData((ESkillGroup)row[24].get<int>(), row[25].get<int>());
		info.Skills.R = FSkillData((ESkillGroup)row[26].get<int>(), row[27].get<int>());

		return { DBResultCode::SUCCESS, dbId, info };
	}
	catch (const mysqlx::Error& e) {
		LOG(LogType::Error, std::format("MySQL Error (CheckLogin - login_id: {}): {}", login_id, e.what()));
		return { DBResultCode::DB_ERROR };
	}
}

bool DBManager::UpdatePlayerInfo(uint32 dbId, const FInfoData& info)
{
	try {
		auto table = _db->getTable("player_info");
		table.update()
			.set("character_type", info.CharacterType)
			.set("pos_x", info.Pos.X)
			.set("pos_y", info.Pos.Y)
			.set("pos_z", info.Pos.Z)
			.set("yaw", info.Yaw)
			.set("collision_radius", info.CollisionRadius)
			.set("attack_radius", info.AttackRadius)
			.set("fov_angle", info.fovAngle)
			.set("level", info.Stats.Level)
			.set("exp", info.Stats.Exp)
			.set("max_exp", info.Stats.MaxExp)
			.set("hp", info.Stats.Hp)
			.set("max_hp", info.Stats.MaxHp)
			.set("damage", info.Stats.Damage)
			.set("crt_rate", info.Stats.CrtRate)
			.set("crt_value", info.Stats.CrtValue)
			.set("dodge", info.Stats.Dodge)
			.set("speed", info.Stats.Speed)
			.set("gold", info.Gold)
			.set("skill1_gid", static_cast<int>(info.Skills.Q.SkillGID))
			.set("skill1_level", info.Skills.Q.SkillLevel)
			.set("skill2_gid", static_cast<int>(info.Skills.E.SkillGID))
			.set("skill2_level", info.Skills.E.SkillLevel)
			.set("skill3_gid", static_cast<int>(info.Skills.R.SkillGID))
			.set("skill3_level", info.Skills.R.SkillLevel)
			.where("id = :id")
			.bind("id", dbId)
			.execute();

		LOG(std::format("Update DB - dbid: {}", dbId));
		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG(LogType::Error, std::format("MySQL Error (UpdatePlayerInfo): {}", e.what()));
		return false;
	}
}


mysqlx::Table DBManager::GetUsersTable()
{
	return _db->getTable(USERS_TABLE);
}