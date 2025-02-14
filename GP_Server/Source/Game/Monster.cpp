#include "pch.h"
#include "Monster.h"
#include "Proto.h"
#include "SessionManager.h"
#include "GameManager.h"
#include <random>
static std::random_device rd;
static std::mt19937 gen(rd());

void Monster::Init()
{
	Character::Init();
	static std::uniform_int_distribution<int> randomType(Type::EMonster::ENERGY_DRINK, Type::EMonster::MOUSE);

	_info.CharacterType = static_cast<ECharacterType>(randomType(gen));
	_info.Stats.Level = _info.CharacterType;
}

void Monster::Update()
{
	if (!IsValid()) return;
	if (IsDead())
	{
		_info.AddState(ECharacterStateType::STATE_DIE);
		return;
	}

	BehaviorTree();
}

void Monster::BehaviorTree()
{
	switch (_info.State)
	{
	case ECharacterStateType::STATE_IDLE:
		if (ShouldWalking())
		{
			ChangeState(ECharacterStateType::STATE_WALK);
		}
		else if (ShouldAttack())
		{
			ChangeState(ECharacterStateType::STATE_AUTOATTACK);
		}
		break;

	case ECharacterStateType::STATE_WALK:
		if (ShouldAttack())
		{
			ChangeState(ECharacterStateType::STATE_AUTOATTACK);
		}
		else if (!ShouldWalking())
		{
			ChangeState(ECharacterStateType::STATE_IDLE);
		}
		break;

	case ECharacterStateType::STATE_AUTOATTACK:
		if (!ShouldAttack())
		{
			ChangeState(ECharacterStateType::STATE_IDLE);
		}
		break;
	}
}

void Monster::ChangeState(ECharacterStateType newState)
{
	std::lock_guard<std::mutex> lock(_cMutex);

	if (!_info.HasState(newState))
	{
		_info.AddState(newState);
	}
}

bool Monster::ShouldAttack()
{
	//Todo: 공격 범위 안에 플레이어 있는지
	static std::uniform_int_distribution<int> dist(0, 1);
	return (dist(gen) == 0);
}

bool Monster::ShouldWalking()
{
	std::lock_guard<std::mutex> lock(_cMutex);
	static std::uniform_int_distribution<int> dist(0, 2);

	if (dist(gen) == 0)
	{
		FVector newPos = GenerateRandomNearbyPosition();

		_info.SetLocationAndYaw(newPos);
		return true;
	}
	return false;
}

FVector Monster::GenerateRandomNearbyPosition()
{
	static std::uniform_real_distribution<float> distX(-1000.0f, 1000.0f);
	static std::uniform_real_distribution<float> distY(-1000.0f, 1000.0f);

	return _info.Pos + FVector(distX(gen), distY(gen), 0);
}
