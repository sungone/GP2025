#include "pch.h"
#include "Monster.h"
#include "SessionManager.h"
#include "GameManager.h"

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
	return RandomUtils::Chance(50);
}

bool Monster::ShouldWalking()
{
	std::lock_guard<std::mutex> lock(_cMutex);

	if (RandomUtils::Chance(50))
	{
		FVector newPos = GenerateRandomNearbyPosition();

		_info.SetLocationAndYaw(newPos);
		return true;
	}
	return false;
}

FVector Monster::GenerateRandomNearbyPosition()
{
	auto distX = RandomUtils::GetRandomFloat(-1000.0f, 1000.0f);
	auto distY = RandomUtils::GetRandomFloat(-1000.0f, 1000.0f);
	return _info.Pos + FVector(distX, distY, 0);
}
