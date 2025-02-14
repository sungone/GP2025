#include "pch.h"
#include "GameManager.h"

bool GameManager::Init()
{
	CreateMonster();
	StartMonsterStateBroadcast();
	UpdateMonster();
	return true;
}

void GameManager::AddPlayer(std::shared_ptr<Character> player)
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	int32 id = player->GetInfo().ID;
	_characters[id] = player;

}

void GameManager::RemoveCharacter(int32 id)
{
	LOG(Log, std::format("Remove Character"));
	if (id < 0 || id >= MAX_CHARACTER || !_characters[id])
	{
		LOG(Warning, "Invalid");
		return;
	}

	if (id < MAX_PLAYER)
	{
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_PLAYER, _characters[id]->GetInfo());
		SessionManager::GetInst().Broadcast(&Pkt);
	}
	else
	{
		auto Pkt = InfoPacket(EPacketType::S_REMOVE_MONSTER, _characters[id]->GetInfo());
		SessionManager::GetInst().Broadcast(&Pkt);
	}

	_characters[id] = nullptr;
}

void GameManager::CreateMonster()
{
	for (int32 i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		_characters[i] = std::make_shared<Monster>();
		_characters[i]->Init();
		_characters[i]->GetInfo().ID = i;
	}
}

void GameManager::SpawnMonster(Session& session)
{
	std::lock_guard<std::mutex> lock(_carrMutex);

	for (int32 i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (_characters[i] && _characters[i]->IsValid())
		{
			auto Pkt = InfoPacket(EPacketType::S_ADD_MONSTER, _characters[i]->GetInfo());
			session.DoSend(&Pkt);
		}
	}
}

void GameManager::ProcessAttack(int32 attackerID, int32 targetID)
{
	std::unique_lock<std::mutex> lock(_carrMutex);
	if (targetID == -1)
		return;

	LOG(Log, std::format("Attacked monster[{}]", targetID));

	auto& Attacker = _characters[attackerID];
	std::shared_ptr<Monster> Target = static_pointer_cast<Monster>(_characters[targetID]);

	if (!CollisionUtils::CanAttack(Attacker->GetInfo(), Target->GetInfo()))
		return;

	float atkDamage = Attacker->GetAttackDamage();
	if (atkDamage > 0.0f)
		Target->OnDamaged(atkDamage);

	auto pkt = DamagePacket(Target->GetInfo(), atkDamage);
	SessionManager::GetInst().Broadcast(&pkt);
}

std::shared_ptr<Character> GameManager::GetCharacterByID(int32 id)
{
	std::lock_guard<std::mutex> lock(_carrMutex);

	if (id < 0 || id >= MAX_CHARACTER || !_characters[id] || !_characters[id]->IsValid())
	{
		LOG(Warning, "Invalid");
		return nullptr;
	}
	return _characters[id];
}

void GameManager::StartMonsterStateBroadcast()
{
	_MonsterStateBroadcastTimer.Start(3000, [this]() {
		BroadcastMonsterStates();
		});
}

void GameManager::BroadcastMonsterStates()
{
	std::lock_guard<std::mutex> lock(_carrMutex);
	LOG(SendLog, std::format("Broadcast monster"));

	for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
	{
		if (_characters[i] && _characters[i]->IsValid())
		{
			auto& monster = _characters[i];
			FInfoData MonsterInfoData = monster->GetInfo();
			InfoPacket packet(S_MONSTER_STATUS_UPDATE, MonsterInfoData);
			SessionManager::GetInst().Broadcast(&packet);
		}
	}
}

void GameManager::UpdateMonster()
{
	_MonsterAIUpdateTimer.Start(4000, [this]() {
		std::unique_lock<std::mutex> lock(_carrMutex);
		for (int i = MAX_PLAYER; i < MAX_CHARACTER; ++i)
		{
			if (!_characters[i]) return;

			_characters[i]->Update();

			if (_characters[i]->IsDead())
			{
				RemoveCharacter(i);
			}
		}
		});
}