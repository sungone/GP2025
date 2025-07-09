#include "pch.h"
#include "Monster.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Monster::Init()
{
	Character::Init();

	auto data = MonsterTable::GetInst().GetMonsterByTypeId(static_cast<uint8>(_monType));
	if (!data)
	{
		LOG(Warning, "Invaild");
		return;
	}
	FVector newPos{};

	//_info.SetName(ConvertToWString(data->Name) + std::to_wstring(_id));
	_info.SetName(ConvertToWString(data->Name));
	_info.CharacterType = data->TypeId;
	_info.Stats.Level = data->Level;
	_info.Stats.Hp = data->Hp;
	_info.Stats.MaxHp = data->Hp;
	_info.Stats.Damage = data->Atk;
	_info.Stats.CrtRate = data->CrtRate;
	_info.Stats.Dodge = data->Dodge;
	_info.Stats.Speed = data->MoveSpd;
	_info.CollisionRadius = data->CollisionRadius;
	_info.AttackRadius = data->AtkRadius;
	_info.State = ECharacterStateType::STATE_IDLE;
	_navMesh = &Map::GetInst().GetNavMesh(_zone);
}

void Monster::UpdateViewList(std::shared_ptr<Character> other)
{
	if (!other) { LOG(Warning, "Invaild!"); return; }

	auto player = std::dynamic_pointer_cast<Player>(other);
	if (!player) return;
	auto playerId = other->GetInfo().ID;
	auto self = std::static_pointer_cast<Character>(shared_from_this());

	bool added = false;
	{
		std::lock_guard lock(_vlLock);
		if (IsInViewDistance(other->GetInfo().Pos, VIEW_DIST))
			added = AddToViewList(playerId);
		else
			added = !RemoveFromViewList(playerId);
	}

	if (!added) return;

	if (IsInViewDistance(other->GetInfo().Pos, VIEW_DIST))
		player->AddMonsterToViewList(self);
	else
		player->RemoveMonsterFromViewList(self);
}

void Monster::Update()
{
	if (!IsActive()) return;

	if (IsDead())
	{
		ChangeState(ECharacterStateType::STATE_DIE);
		SetActive(false);
		return;
	}

	BehaviorTree();
}

void Monster::BehaviorTree()
{
	if (_info.HasState(ECharacterStateType::STATE_DIE))
		return;

	if (_info.HasState(ECharacterStateType::STATE_SKILL_Q) || _info.HasState(ECharacterStateType::STATE_SKILL_E))
		return; // 스킬 중이면 아무것도 안함

	if (_info.HasState(ECharacterStateType::STATE_AUTOATTACK))
	{
		if (!_target)
		{
			ChangeState(ECharacterStateType::STATE_IDLE);
		}
		else if (!IsTargetInAttackRange())
		{
			ChangeState(ECharacterStateType::STATE_WALK);
			Chase();
		}
		else
		{
			Attack();
		}
		return;
	}

	if (_info.HasState(ECharacterStateType::STATE_WALK))
	{
		if (_target)
			Chase();
		else if (SetTarget())
			Chase();
		else
			Patrol();
		return;
	}

	if (_info.HasState(ECharacterStateType::STATE_IDLE))
	{
		if (SetTarget())
		{
			Chase();
			ChangeState(ECharacterStateType::STATE_WALK);
		}
		else if (RandomUtils::GetRandomBool())
		{
			ChangeState(ECharacterStateType::STATE_WALK);
		}
		return;
	}

	LOG(Warning, std::format("Invalid state bits: {}", static_cast<uint32>(_info.State)));
}


void Monster::Look()
{
	if (!_target) return;
	float yaw = _info.CalculateYaw(_target->GetInfo().Pos);
	_info.SetYaw(yaw);
}

void Monster::Attack()
{
	ChangeState(ECharacterStateType::STATE_AUTOATTACK);

	if (!_target || _target->IsDead()) return;
	if (IsBoss())
	{
		BossAttack();
	}
	else
		PerformMeleeAttack();
}

void Monster::BossAttack()
{
	switch (_currentPattern)
	{
	case EAttackPattern::MeleeAttack:
		PerformMeleeAttack();
		break;
	case EAttackPattern::FlameBreath:
		_info.AddState(ECharacterStateType::STATE_SKILL_Q);
		PerformFlameBreath();
		break;
	case EAttackPattern::EarthQuake:
		_info.AddState(ECharacterStateType::STATE_SKILL_E);
		PerformEarthQuake();
		break;
	}

	SetNextPattern();
}

void Monster::PerformEarthQuake()
{
	LOG("EarthQuake!");

	int rockCount = 5;
	if (_info.GetHp() / _info.GetMaxHp() < 0.3f)
		rockCount = 8;

	for (int i = 0; i < rockCount; ++i)
	{
		FVector rockPos = _target->GetInfo().Pos + RandomUtils::GetRandomOffset(0, 500, 0);

		std::unordered_set<int32> viewListCopy;
		{
			std::lock_guard lock(_vlLock);
			viewListCopy = GetViewList();
		}
		auto pkt = Tino::EarthQuakePacket(rockPos);
		SessionManager::GetInst().BroadcastToViewList(&pkt, viewListCopy);
		auto id = _id;
		TimerQueue::AddTimer([rockPos, id] {
			GameWorld::GetInst().HandleEarthQuakeImpact(rockPos);
			GameWorld::GetInst().UpdateMonsterState(id, ECharacterStateType::STATE_IDLE);
			}, 500, false);
	}
}

void Monster::PerformFlameBreath()
{
	LOG("FlameBreath Start!");

	const float range = _info.AttackRadius;
	const float halfAngleDeg = 15.f;
	const float maxDamage = 40.f;
	const int delayMs = 500; // 이펙트 먼저 보여주고 0.5초 후 판정

	FVector origin = _pos;
	FVector forward = _info.GetFrontVector().Normalize();

	std::unordered_set<int32> viewListCopy;
	{
		std::lock_guard lock(_vlLock);
		viewListCopy = GetViewList();
	}

	auto flamePkt = Tino::FlameBreathPacket(origin, forward, range, halfAngleDeg * 2);
	SessionManager::GetInst().BroadcastToViewList(&flamePkt, viewListCopy);

	int monsterId = _id;

	TimerQueue::AddTimer([=]() {
		auto monster = GameWorld::GetInst().GetMonsterByID(monsterId);
		if (!monster) return;

		for (int32 pid : viewListCopy)
		{
			auto player = GameWorld::GetInst().GetPlayerByID(pid);
			if (!player || player->IsDead()) continue;

			FVector toTarget = player->GetInfo().Pos - origin;
			toTarget.Z = 0.0f;

			if (toTarget.LengthSquared() > range * range) continue;

			FVector toTargetNorm = toTarget.Normalize();
			float dot = forward.DotProduct(toTargetNorm);
			float angle = std::acos(dot) * (180.0f / 3.14159265f);
			if (angle > halfAngleDeg) continue;

			float dist = std::sqrt(toTarget.LengthSquared());
			float ratio = 1.0f - (dist / range);
			float damage = maxDamage * ratio;

			LOG(std::format("FlameBreath HIT [{}] dmg: {:.1f}", pid, damage));
			player->OnDamaged(damage);
			auto hitDebugPkt = Tino::FlameBreathPacket(origin, forward, range, halfAngleDeg * 2, true);
			SessionManager::GetInst().BroadcastToViewList(&hitDebugPkt, viewListCopy);
		}

		GameWorld::GetInst().UpdateMonsterState(monsterId, ECharacterStateType::STATE_IDLE);
		}, delayMs, false);
}



void Monster::PerformFlameBreathRotate()
{
	LOG("Rotating FlameBreath!");
	const float range = _info.AttackRadius;
	const float halfAngleDeg = 15.f;
	const float maxDamage = 40.f;
	const int tickCount = 10;
	const float tickIntervalMs = 100.f;
	const float rotationPerTickDeg = 9.f;

	FVector origin = _pos;
	FVector forward = _info.GetFrontVector().Normalize();
	forward.Z = 0.0f;

	float startYawRad = DegreesToRadians(-180.f);

	std::unordered_set<int32> viewListCopy;
	{
		std::lock_guard lock(_vlLock);
		viewListCopy = GetViewList();
	}

	int32 monsterId = _id;

	for (int i = 0; i < tickCount; ++i)
	{
		TimerQueue::AddTimer([=]() {
			auto monster = GameWorld::GetInst().GetMonsterByID(monsterId);
			if (!monster) return;

			// 현재 틱의 회전 각도 (시계방향으로 감소)
			float yaw = startYawRad - DegreesToRadians(rotationPerTickDeg * i);
			FVector dir(std::cos(yaw), std::sin(yaw), 0.f);

			for (int32 pid : viewListCopy)
			{
				auto player = GameWorld::GetInst().GetPlayerByID(pid);
				if (!player || player->IsDead()) continue;

				FVector toTarget = player->GetInfo().Pos - origin;
				toTarget.Z = 0.f;

				float distSq = toTarget.LengthSquared();
				if (distSq > range * range) continue;

				FVector toTargetNorm = toTarget.Normalize();
				float dot = dir.DotProduct(toTargetNorm);
				float angleToTarget = std::acos(dot) * (180.0f / 3.14159265f);

				if (angleToTarget > halfAngleDeg) continue;

				float dist = std::sqrt(distSq);
				float ratio = 1.0f - (dist / range);
				float damage = maxDamage * ratio;

				LOG(std::format("[Tick {}] FlameBreath hit [{}] - dist: {:.1f}, dmg: {:.1f}", i, pid, dist, damage));
				player->OnDamaged(damage);
			}

			auto flamePkt = Tino::FlameBreathPacket(origin, dir, range, halfAngleDeg * 2);
			SessionManager::GetInst().BroadcastToViewList(&flamePkt, viewListCopy);

			}, i * tickIntervalMs, false);
	}

	TimerQueue::AddTimer([monsterId] {
		GameWorld::GetInst().UpdateMonsterState(monsterId, ECharacterStateType::STATE_IDLE);
		}, tickCount * tickIntervalMs, false);
}


void Monster::PerformMeleeAttack()
{
	if (IsTargetInAttackRange())
	{
		_target->OnDamaged(GetAttackDamage());
	}
}

void Monster::SetNextPattern()
{

	int randomIndex = RandomUtils::GetRandomInt(0, (GetMonsterType() == Type::EMonster::TINO) ? 2 : 1);
	_currentPattern = static_cast<EAttackPattern>(randomIndex);
}


void Monster::Chase()
{
	if (!_target) return;

	if (!IsTargetInChaseRange())
	{
		LOG("Is Not In ChaseRange!");
		_target.reset();
		ChangeState(ECharacterStateType::STATE_IDLE);
		return;
	}

	LOG("Chase!");
	//todo: 길찾기로 처리 하자
	Look();
	ChangeState(ECharacterStateType::STATE_AUTOATTACK);
}

void Monster::Patrol()
{

}


bool Monster::SetTarget()
{
	std::vector<int32> viewListCopy;
	{
		std::lock_guard lock(_vlLock);
		viewListCopy.assign(_viewList.begin(), _viewList.end());
	}

	for (auto& playerId : viewListCopy)
	{
		if (playerId >= MAX_PLAYER)
		{
			LOG(Warning, "Invaild");
			ChangeState(ECharacterStateType::STATE_IDLE);
			continue;
		}
		auto player = GameWorld::GetInst().GetPlayerByID(playerId);
		if (!player)
		{
			RemoveFromViewList(playerId);
			continue;
		}
		if (IsInViewDistance(player->GetInfo().Pos, detectDist))
		{
			LOG("SetTarget!");
			//Todo: 제일 가까운 플레이어를 쫓아야함...
			ChangeState(ECharacterStateType::STATE_WALK);
			_target = std::dynamic_pointer_cast<Player>(player);
			return true;
		}
	}
	return false;
}

bool Monster::IsTargetInAttackRange()
{
	if (_target != nullptr)
	{
		return this->IsInAttackRange(_target->GetInfo());
	}
	return false;
}

bool Monster::IsTargetInChaseRange()
{
	if (_target != nullptr)
	{
		return this->IsInViewDistance(_target->GetInfo().Pos, detectDist);
	}
	return false;
}
