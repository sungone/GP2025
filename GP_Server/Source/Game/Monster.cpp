#include "pch.h"
#include "Monster.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Monster::Init()
{
	Character::Init();
	_characterType = CharacterType::Monster;

	FVector newPos{};
	do { newPos = MapZone::GetInst().GetRandomPos(ZoneType::DEFAULT); } while (GameWorld::GetInst().IsCollisionDetected(newPos));
	_info.SetLocation(newPos);

	_info.CharacterType = RandomUtils::GetRandomUint8((uint8)Type::EMonster::ENERGY_DRINK, (uint8)Type::EMonster::TINO);
	_info.Stats.Level = _info.CharacterType;
	_info.CollisionRadius = 100.f;
	_info.AttackRadius = 200;
}

void Monster::UpdateViewList(std::shared_ptr<Character> other)
{
	if (!other) { LOG(Warning, "Invaild!"); return; }

	auto otherId = other->GetInfo().ID;

	if (IsInViewDistance(other->GetInfo().Pos, VIEW_DIST))
	{
		AddToViewList(otherId);
	}
	else
	{
		RemoveFromViewList(other->GetInfo().ID);
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
		if (DetectTarget())
		{
			ChangeState(ECharacterStateType::STATE_AUTOATTACK);
		}
		else if (RandomUtils::GetRandomBool())
		{
			ChangeState(ECharacterStateType::STATE_WALK);
		}
		break;
	case ECharacterStateType::STATE_WALK:
		if (IsTargetInRange())
		{
			ChangeState(ECharacterStateType::STATE_AUTOATTACK);
		}
		break;
	case ECharacterStateType::STATE_AUTOATTACK:
		if (!IsTargetInRange())
		{
			ChangeState(ECharacterStateType::STATE_IDLE);
		}
		break;
	}
}

void Monster::ChangeState(ECharacterStateType newState)
{
	if (!_info.HasState(newState))
	{
		_info.AddState(newState);
	}
}

bool Monster::DetectTarget()
{

	return false;
}

bool Monster::IsTargetInRange()
{
	if (_target != nullptr)
	{
		return this->IsInAttackRange(_target->GetInfo());
	}
	return false;
}
