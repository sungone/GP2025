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
	switch (_info.State)
	{
	case ECharacterStateType::STATE_IDLE:
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
		break;
	}
	case ECharacterStateType::STATE_WALK:
	{
		if (_target)
		{
			Chase();
			break;
		}

		if (SetTarget())
		{
			Chase();
		}
		else
		{
			Patrol();
		}
		break;
	}
	case ECharacterStateType::STATE_AUTOATTACK:
	{
		if (!_target)
		{
			ChangeState(ECharacterStateType::STATE_IDLE);
			break;
		}

		if (!IsTargetInAttackRange())
		{
			ChangeState(ECharacterStateType::STATE_WALK);
			Chase();
		}
		else
		{
			Attack();
		}
		break;
	}
	default:
		LOG(Warning, std::format("Invaild State :{}", _info.State));
	}
}

void Monster::Look()
{
	if (!_target) return;
	float yaw = _info.CalculateYaw(_target->GetInfo().Pos);
	_info.SetYaw(yaw);
}

void Monster::Attack()
{
	if (!_target || _target->IsDead()) return;
	if (GetMonsterType() == Type::EMonster::TINO)
	{
		TinoAttack();
	}
	else
		PerformMeleeAttack();
}

void Monster::TinoAttack()
{
	PerformEarthQuake();

	//switch (_currentPattern)
	//{
	//case EAttackPattern::EarthQuake:
	//	PerformEarthQuake();
	//	break;
	//case EAttackPattern::FlameBreath:
	//	PerformFlameBreath();
	//	break;
	//case EAttackPattern::MeleeAttack:
	//	PerformMeleeAttack();
	//	break;
	//}

	//SetNextPattern();
}

void Monster::PerformEarthQuake()
{
	LOG("EarthQuake!");

	int rockCount = 5;
	if (_info.GetHp() / _info.GetMaxHp() < 0.3f)
		rockCount = 8;

	for (int i = 0; i < rockCount; ++i)
	{
		FVector rockPos = _target->GetInfo().Pos + RandomUtils::GetRandomOffset(0, 300, 0);

		std::unordered_set<int32> viewListCopy;
		{
			std::lock_guard lock(_vlLock);
			viewListCopy = GetViewList();
		}
		auto pkt = Tino::EarthQuakePacket(rockPos);
		SessionManager::GetInst().BroadcastToViewList(&pkt, viewListCopy);

		TimerQueue::AddTimer([rockPos] { GameWorld::GetInst().HandleEarthQuakeImpact(rockPos);}, 500, false);
	}
}

void Monster::PerformFlameBreath()
{
	LOG("FlameBreath!");

	const float range = 600.f;
	const float angleDeg = 30.f;
	const float maxDamage = 40.f;

	FVector origin = _pos;
	FVector forward = _info.GetFrontVector();

	std::unordered_set<int32> viewListCopy;
	{
		std::lock_guard lock(_vlLock);
		viewListCopy = GetViewList();
	}

	for (int32 id : viewListCopy)
	{
		auto player = GameWorld::GetInst().GetPlayerByID(id);
		if (!player || player->IsDead()) continue;

		const FVector& targetPos = player->GetInfo().Pos;
		FVector toTarget = (targetPos - origin);
		float distance = toTarget.Length();
		if (distance > range) continue;

		FVector toTargetNorm = toTarget.Normalize();
		float dot = forward.DotProduct(toTargetNorm);
		float angleToTarget = std::acos(dot) * (180.0f / 3.14159265f);

		if (angleToTarget > angleDeg / 2.0f) continue;

		float ratio = 1.0f - (distance / range);
		float damage = maxDamage * ratio;

		//player->OnDamaged(damage);

		LOG(std::format("FlameBreath hit player [{}] - dist: {:.1f}, damage: {:.1f}", id, distance, damage));
		auto flamePkt = Tino::FlameBreathPacket(origin, forward, range, angleDeg);
		SessionManager::GetInst().BroadcastToViewList(&flamePkt, viewListCopy);
	}
}


void Monster::PerformMeleeAttack()
{
	if (IsTargetInAttackRange())
	{
		//_target->OnDamaged(GetAttackDamage());
	}
}

void Monster::SetNextPattern()
{
	int randomIndex = RandomUtils::GetRandomInt(0, 2);
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
