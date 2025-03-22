#include "pch.h"
#include "Monster.h"
#include "SessionManager.h"
#include "GameWorld.h"

void Monster::Init()
{
	Character::Init();
	SetParkingGarageRandomLocation();
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

void Monster::SetTargetIdx(const FVector& TargetPosition)
{
	int TargetPolygon = NavMesh::FindIdxFromPos(TargetPosition);
	_navPath = NavMesh::FindPath(_curPolyIdx, TargetPolygon);
	_curPathIndex = 0;
}

void Monster::SetTarget(const FVector& TargetPosition)
{
	_targetPos = TargetPosition;
	_hasTarget = true;

	int TargetPolygon = NavMesh::FindIdxFromPos(TargetPosition);
	_navPath = NavMesh::FindPath(_curPolyIdx, TargetPolygon);
	_curPathIndex = 0;
}

void Monster::SetRandomPatrol()
{
	FVector RandomTarget = _pos + FVector(RandomUtils::GetRandomFloat(-1000, 1000), RandomUtils::GetRandomFloat(-1000, 1000), 0);
	SetTarget(RandomTarget);
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
	// Ÿ�ٿ��� �������� �����ų� ������ �����ϴ� �ڵ� �ۼ�
}


bool Monster::DetectTarget()
{
	// Ÿ�� ���� ������ �߰�
	return false;
}

bool Monster::IsTargetInRange()
{
	// ���� Ÿ�ٰ��� �Ÿ��� ����Ͽ� ���� ���� �ȿ� �ִ��� Ȯ��
	return false;
}

bool Monster::IsPathComplete()
{
	return _curPathIndex >= _navPath.size();
}
