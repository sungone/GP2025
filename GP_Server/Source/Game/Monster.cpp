#include "pch.h"
#include "Monster.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Monster::Init()
{
	Character::Init();
	_characterType = CharacterType::Monster;

	FVector newPos{};
	do { newPos = MapZone::GetInst().GetRandomPos(ZoneType::PLAYGROUND); } while (GameWorld::GetInst().IsCollisionDetected(newPos));
	_info.SetLocation(newPos);

	_info.CharacterType = RandomUtils::GetRandomUint8((uint8)Type::EMonster::ENERGY_DRINK, (uint8)Type::EMonster::TINO);
	_info.Stats.Level = _info.CharacterType;
	_info.CollisionRadius = 100.f;
	_info.AttackRadius = 150;
	_info.Speed = 200.f;
	_info.State = ECharacterStateType::STATE_IDLE;
}

void Monster::UpdateViewList(std::shared_ptr<Character> other)
{
	std::lock_guard<std::mutex> lock(_vlLock);
	if (!other) { LOG(Warning, "Invaild!"); return; }
	auto player = std::dynamic_pointer_cast<Player>(other);
	if (!player) return;
	auto playerId = other->GetInfo().ID;

	if (IsInViewDistance(other->GetInfo().Pos, VIEW_DIST))
	{
		if (!AddToViewList(playerId)) return;
		player->AddMonsterToViewList(std::shared_ptr<Character>(this));
	}
	else
	{
		if (!RemoveFromViewList(playerId)) return;
		player->RemoveMonsterFromViewList(std::shared_ptr<Character>(this));
	}
}

void Monster::Update()
{
	if (IsDead())
	{
		ChangeState(ECharacterStateType::STATE_DIE);
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
			Look();
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
			Look();
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
	if (!_target) return;

	auto player = _target;
	auto playerID = player->GetInfo().ID;

	float atkDamage = this->GetAttackDamage();
	if (atkDamage > 0.0f)
	{
		player->OnDamaged(atkDamage);
	}

	auto pkt = InfoPacket(EPacketType::S_DAMAGED_PLAYER, player->GetInfo());
	SessionManager::GetInst().SendPacket(playerID, &pkt);

	if (player->IsDead())
	{
		//Todo: 플레이어 죽음처리
	}
}

void Monster::Chase()
{
	if (!_target) return;

	if (!IsTargetInChaseRange())
	{
		LOG("Is Not In ChaseRange!");

		_target.reset();
		return;
	}
	LOG("Chase!");
	auto playerPos = _target->GetInfo().Pos;
	FVector dir = (playerPos - _pos).Normalize();
	FVector dist = dir * _info.Speed;
	if (dist.Length() < _pos.DistanceTo(playerPos) - _info.AttackRadius)
	{
		_info.SetLocationAndYaw(_pos + dist);
	}
	else
	{
		_info.SetLocationAndYaw(playerPos - dir * _info.AttackRadius);
		ChangeState(ECharacterStateType::STATE_AUTOATTACK);
	}
}

void Monster::Patrol()
{
	static const float PatrolRadius = 500.0f;
	float RandomX = RandomUtils::GetRandomFloat(-PatrolRadius, PatrolRadius);
	float RandomY = RandomUtils::GetRandomFloat(-PatrolRadius, PatrolRadius);

	auto newPos = FVector(_pos.X + RandomX, _pos.Y + RandomY, _pos.Z);
	_info.SetLocationAndYaw(newPos);
}

bool Monster::SetTarget()
{
	std::lock_guard<std::mutex> lock(_vlLock);
	for (auto& playerId : this->GetViewList())
	{
		if (playerId >= MAX_PLAYER)
		{
			LOG(Warning, "Invaild");
			continue;
		}
		auto player = GameWorld::GetInst().GetCharacterByID(playerId);
		if (!player) continue;
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
