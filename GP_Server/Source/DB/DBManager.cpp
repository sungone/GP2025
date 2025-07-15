#include "pch.h"
#include "DBManager.h"
#include "GameWorld.h"
#include "ScopedDBSession.h"

bool DBManager::Connect(const std::string& host, const std::string& user, const std::string& pwd, const std::string& db)
{
	return DBConnectionPool::GetInst().Init(host, user, pwd, db, 10);
}

void DBManager::Shutdown()
{
	DBConnectionPool::GetInst().Shutdown();
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
		LOG_W("Invalid level stat");
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
	newinfo.SetLocation(newPos);

	newinfo.CurrentQuest = { QuestType::NONE, EQuestStatus::NotStarted };
	newinfo.EquipState = {};

	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");
		auto result = schema.getTable("users")
			.insert("login_id", "password", "nickname")
			.values(login_id, password, nickname)
			.execute();

		uint32 dbId = static_cast<uint32>(result.getAutoIncrementValue());

		schema.getTable("player_info")
			.insert("id", "character_type", "pos_x", "pos_y", "pos_z", "yaw",
				"collision_radius", "attack_radius", "fov_angle",
				"level", "exp", "max_exp", "hp", "max_hp", "damage",
				"crt_rate", "crt_value", "dodge", "speed", "gold",
				"skill1_gid", "skill1_level",
				"skill2_gid", "skill2_level",
				"skill3_gid", "skill3_level",
				"current_quest_type", "current_quest_status",
				"equip_weapon", "equip_helmet", "equip_chest"
			)
			.values(dbId, newinfo.CharacterType, newPos.X, newPos.Y, newPos.Z, newinfo.Yaw,
				newinfo.CollisionRadius, newinfo.AttackRadius, newinfo.fovAngle,
				stats.Level, stats.Exp, stats.MaxExp, stats.Hp, stats.MaxHp, stats.Damage,
				stats.CrtRate, stats.CrtValue, stats.Dodge, stats.Speed, newinfo.Gold,
				static_cast<int>(newinfo.Skills.Q.SkillGID), newinfo.Skills.Q.SkillLevel,
				static_cast<int>(newinfo.Skills.E.SkillGID), newinfo.Skills.E.SkillLevel,
				static_cast<int>(newinfo.Skills.R.SkillGID), newinfo.Skills.R.SkillLevel,
				static_cast<int>(newinfo.CurrentQuest.QuestType),
				static_cast<int>(newinfo.CurrentQuest.Status),
				static_cast<int>(newinfo.EquipState.Sword),
				static_cast<int>(newinfo.EquipState.Helmet),
				static_cast<int>(newinfo.EquipState.Chest)
			)
			.execute();


		return { DBResultCode::SUCCESS, dbId, newinfo };
	}
	catch (const mysqlx::Error& e)
	{
		std::string msg = e.what();
		if (msg.find("Duplicate entry") != std::string::npos) {
			return { DBResultCode::DUPLICATE_ID };
		}
		LOG_E("MySQL Error: {}", msg);
		return { DBResultCode::DB_ERROR };
	}
}

DBLoginResult DBManager::CheckLogin(int32 sessionId, const std::string& login_id, const std::string& password)
{
	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");
		auto result = sess.sql(
			"SELECT "
			"u.id, u.password, u.nickname, "
			"p.character_type, p.pos_x, p.pos_y, p.pos_z, p.yaw, "
			"p.collision_radius, p.attack_radius, p.fov_angle, "
			"p.level, p.exp, p.max_exp, p.hp, p.max_hp, "
			"p.damage, p.crt_rate, p.crt_value, p.dodge, p.speed, "
			"p.gold, "
			"p.skill1_gid, p.skill1_level, "
			"p.skill2_gid, p.skill2_level, "
			"p.skill3_gid, p.skill3_level, "
			"p.current_quest_type, p.current_quest_status, "
			"p.equip_weapon, p.equip_helmet, p.equip_chest "
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

		info.CurrentQuest.QuestType = static_cast<QuestType>(row[28].get<int>());
		info.CurrentQuest.Status = static_cast<EQuestStatus>(row[29].get<int>());

		info.EquipState.Sword = static_cast<Type::EWeapon>(row[30].get<int>());
		info.EquipState.Helmet = static_cast<Type::EArmor>(row[31].get<int>());
		info.EquipState.Chest = static_cast<Type::EArmor>(row[32].get<int>());

		std::vector<std::pair<uint32, uint8>> itemList;

		auto itemResult = schema.getTable("user_items")
			.select("item_uid", "item_type_id")
			.where("user_id = :uid")
			.bind("uid", dbId)
			.execute();

		for (auto row : itemResult)
		{
			uint32 itemID = static_cast<uint32>(row[0].get<int>());
			uint8 itemTypeID = static_cast<uint8>(row[1].get<int>());
			itemList.emplace_back(itemID, itemTypeID);
		}
		return { DBResultCode::SUCCESS, dbId, info, itemList };
	}
	catch (const mysqlx::Error& e) {
		LOG_E("MySQL Error (CheckLogin - login_id: {}): {}", login_id, e.what());
		return { DBResultCode::DB_ERROR };
	}
}

bool DBManager::UpdatePlayerInfo(uint32 dbId, const FInfoData& info)
{
	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");

		auto table = schema.getTable("player_info");
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
			.set("current_quest_type", static_cast<int>(info.CurrentQuest.QuestType))
			.set("current_quest_status", static_cast<int>(info.CurrentQuest.Status))
			.set("equip_weapon", static_cast<int>(info.EquipState.Sword))
			.set("equip_helmet", static_cast<int>(info.EquipState.Helmet))
			.set("equip_chest", static_cast<int>(info.EquipState.Chest))
			.where("id = :id")
			.bind("id", dbId)
			.execute();

		LOG_D("Update DB - dbid: {}", dbId);
		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("MySQL Error (UpdatePlayerInfo): {}", e.what());
		return false;
	}
}

bool DBManager::AddUserItem(uint32 dbId, uint32 itemID, uint8 itemTypeID)
{
	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");
		schema.getTable("user_items")
			.insert("item_uid", "user_id", "item_type_id")
			.values(itemID, dbId, itemTypeID)
			.execute();

		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("MySQL Error (AddUserItem): {}", e.what());
		return false;
	}
}


bool DBManager::RemoveUserItem(uint32 dbId, uint32 itemID)
{
	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");
		schema.getTable("user_items")
			.remove()
			.where("item_uid = :uid AND user_id = :userid")
			.bind("uid", itemID)
			.bind("userid", dbId)
			.execute();

		LOG_D("Remove Item - userId: {}, itemUID: {}", dbId, itemID);
		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("MySQL Error (RemoveUserItem): {}", e.what());
		return false;
	}
}

bool DBManager::SendFriendRequest(uint32 fromId, uint32 toId)
{
	try {
		// ���� ������� ������ ��û�� ��� X
		if (fromId == toId)
		{
			LOG_W("Cannot send friend request to self: {}", fromId);
			return false;
		}

		// �̹� ģ���̰ų� ��û ������ �˻�
		if (DBManager::GetInst().IsFriendOrPending(fromId, toId))
		{
			LOG_W("Friend request already exists or already friends: {} -> {}", fromId, toId);
			return false;
		}

		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");

		schema.getTable("user_friends")
			.insert("user_id", "friend_id", "status")
			.values(fromId, toId, 0) // 0 = pending
			.execute();

		LOG_D("Friend request sent: {} -> {}", fromId, toId);
		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("MySQL Error (SendFriendRequest): {}", e.what());
		return false;
	}
}

bool DBManager::IsFriendOrPending(uint32 userId, uint32 targetId)
{
	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");

		auto result = schema.getTable("user_friends")
			.select("status")
			.where("user_id = :uid AND friend_id = :fid")
			.bind("uid", userId)
			.bind("fid", targetId)
			.execute();

		return result.count() > 0; // �����ϸ� �̹� ģ���ų� ��û ��
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("MySQL Error (IsFriendOrPending): {}", e.what());
		return false;
	}
}

std::optional<FFriendInfo> DBManager::AcceptFriendRequest(uint32 fromId, uint32 toId)
{
	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");

		// 1. ��û ���¸� 'accepted'�� ����
		auto updateRes = schema.getTable("user_friends")
			.update()
			.set("status", 1)
			.where("user_id = :from AND friend_id = :to AND status = 0")
			.bind("from", fromId)
			.bind("to", toId)
			.execute();

		if (updateRes.getAffectedItemsCount() == 0)
		{
			LOG_W("No pending request found from {} to {}", fromId, toId);
			return std::nullopt;
		}

		// 2. ������ accepted ���� ���� (�ߺ��̸� ����)
		try {
			schema.getTable("user_friends")
				.insert("user_id", "friend_id", "status")
				.values(toId, fromId, 1)
				.execute();
		}
		catch (...) {
			// �̹� �ִٸ� ����
		}

		// 3. ģ�� ���� ��ȯ (fromId ����)
		auto result = sess.sql(
			"SELECT u.id, u.nickname, p.level "
			"FROM users u JOIN player_info p ON u.id = p.id "
			"WHERE u.id = ?"
		).bind(fromId).execute();

		auto row = result.fetchOne();
		if (!row) return std::nullopt;

		FFriendInfo info;
		info.Id = static_cast<uint32>(row[0].get<int>());
		info.Nickname = row[1].get<std::string>();
		info.Level = static_cast<uint32>(row[2].get<int>());
		info.bAccepted = true;

		LOG_D("Friend accepted: {} <-> {}", fromId, toId);
		return info;
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("MySQL Error (AcceptFriendRequest): {}", e.what());
		return std::nullopt;
	}
}

bool DBManager::RemoveFriendRequest(uint32 fromId, uint32 toId)
{
	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");

		auto result = schema.getTable("user_friends")
			.remove()
			.where("user_id = :from AND friend_id = :to AND status = 0")
			.bind("from", fromId)
			.bind("to", toId)
			.execute();

		if (result.getAffectedItemsCount() == 0)
		{
			LOG_W("No pending friend request found from {} to {}", fromId, toId);
			return false;
		}

		LOG_D("Friend request removed: {} -> {}", fromId, toId);
		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("MySQL Error (RemoveFriendRequest): {}", e.what());
		return false;
	}
}

bool DBManager::RemoveFriend(uint32 userId, uint32 friendId)
{
	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();
		auto schema = sess.getSchema("gp2025");

		auto result = schema.getTable("user_friends")
			.remove()
			.where("((user_id = :u1 AND friend_id = :u2) OR (user_id = :u2 AND friend_id = :u1)) AND status = 1")
			.bind("u1", userId)
			.bind("u2", friendId)
			.execute();

		if (result.getAffectedItemsCount() == 0)
		{
			LOG_W("No accepted friendship found between {} and {}", userId, friendId);
			return false;
		}

		LOG_D("Friendship removed: {} <-> {}", userId, friendId);
		return true;
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("MySQL Error (RemoveFriend): {}", e.what());
		return false;
	}
}

std::vector<FFriendInfo> DBManager::GetFriendList(uint32 userId)
{
	std::vector<FFriendInfo> friendList;

	try {
		ScopedDBSession scoped;
		auto& sess = scoped.Get();

		// JOIN: user_friends �� users �� player_info
		auto result = sess.sql(
			"SELECT f.friend_id, u.nickname, p.level, f.status "
			"FROM user_friends f "
			"JOIN users u ON f.friend_id = u.id "
			"JOIN player_info p ON f.friend_id = p.id "
			"WHERE f.user_id = ?"
		).bind(userId).execute();

		for (const auto& row : result)
		{
			FFriendInfo info;
			info.Id = static_cast<uint32>(row[0].get<int>());
			info.Nickname = row[1].get<std::string>();
			info.Level = static_cast<uint32>(row[2].get<int>());
			info.bAccepted = row[3].get<int>() == 1;

			friendList.emplace_back(info);
		}

		LOG_D("Loaded {} friend(s) for user {}", friendList.size(), userId);
	}
	catch (const mysqlx::Error& e)
	{
		LOG_E("MySQL Error (GetFriendList): {}", e.what());
	}

	return friendList;
}
