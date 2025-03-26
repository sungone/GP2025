#include "pch.h"
#include "Character.h"
#include "GameWorld.h"

void Character::Init()
{
	_info.SetYaw(RandomUtils::GetRandomFloat(-180, 180));
}

bool Character::IsDead()
{
	return _info.IsDead();
}

void Character::OnDamaged(float damage)
{
	_info.TakeDamage(damage);
	if (IsDead())
	{
		_info.AddState(ECharacterStateType::STATE_DIE);
	}
}

float Character::GetAttackDamage()
{
	return _info.GetAttackDamage(RandomUtils::GetRandomFloat(0.0f, 1.0f));
}

bool Character::IsColision(const FVector& pos)
{
	return _info.Pos.IsInRange(pos, _info.CollisionRadius);
}

bool Character::IsInAttackRange(const FInfoData& target)
{
	return IsInViewDistance(target.Pos, _info.AttackRadius + target.CollisionRadius);
}

bool Character::IsInViewDistance(const FVector& targetPos, float viewDist)
{
	return _info.Pos.IsInRange(targetPos, viewDist);
}

bool Character::HasLineOfSight(const FVector& targetPos, const std::vector<FVector>& obstacles)
{
	for (const auto& obstacle : obstacles)
	{
		if (_info.Pos.IsInRange(obstacle, _info.Pos.DistanceSquared(targetPos) / 2))
		{
			return false;
		}
	}
	return true;
}

bool Character::AddToViewList(int32 CharacterId)
{
	if (!_viewList.contains(CharacterId))
	{
		_viewList.insert(CharacterId);
		return true;
	}
	return false;
}

bool Character::RemoveFromViewList(int32 CharacterId)
{
	if (_viewList.contains(CharacterId))
	{
		_viewList.erase(CharacterId);
		return true;
	}
	return false;
}