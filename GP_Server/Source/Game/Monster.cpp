#include "pch.h"
#include "Monster.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Monster::Init()
{
	Character::Init();
	_info.CharacterType = RandomUtils::GetRandomUint8((uint8)Type::EMonster::ENERGY_DRINK, (uint8)Type::EMonster::TINO);
	_info.Stats.Level = _info.CharacterType;
}

void Monster::Update()
{
	if (!IsValid()) return;

	if (IsDead())
	{
		ChangeState(ECharacterStateType::STATE_DIE);
		return;
	}

	BehaviorTree();
	Move();
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
			SetRandomPatrol();
			ChangeState(ECharacterStateType::STATE_WALK);
		}
		break;
	case ECharacterStateType::STATE_WALK:
		if (IsTargetInRange())
		{
			ChangeState(ECharacterStateType::STATE_AUTOATTACK);
		}
		else if (IsPathComplete())
		{
			ChangeState(ECharacterStateType::STATE_IDLE);
		}
		break;
	case ECharacterStateType::STATE_AUTOATTACK:
        if (!IsTargetInRange())
        {
            ChangeState(ECharacterStateType::STATE_IDLE);
        }
        else
        {
            Attack();
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

void Monster::SetTarget(const FVector& TargetPos)
{
	_targetPos = TargetPos;
	_hasTarget = true;

	int TargetPolygon = NavMesh::FindIdxFromPos(TargetPos);
	_navPath = NavMesh::FindPath(_curPolyIdx, TargetPolygon);
	_curPathIndex = 0;
}

void Monster::SetRandomPatrol()
{
	//Todo
}

void Monster::Move()
{
	if (_curPathIndex < _navPath.size()) {
		int NextPolygonIndex = _navPath[_curPathIndex];
		_pos = NavMesh::Vertices[NavMesh::Triangles[NextPolygonIndex].IndexA];
		_curPolyIdx = NextPolygonIndex;
		_curPathIndex++;
	}
}

void Monster::Attack()
{
	// Todo:
}


bool Monster::DetectTarget()
{
	// Todo:

	return true;
}

bool Monster::IsTargetInRange()
{
	// Todo:

	return true;
}

bool Monster::IsPathComplete()
{
	return _curPathIndex >= _navPath.size();
}
