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

bool Character::IsColision(const FVector& pos, float dist)
{
	return _info.Pos.IsInRange(pos, dist);
}

bool Character::IsColision(const FInfoData& target)
{
	return _info.Pos.IsInRange(target.Pos, _info.CollisionRadius + target.CollisionRadius);
}

bool Character::IsInAttackRange(const FInfoData& target)
{
	return IsInViewDistance(target.Pos, _info.AttackRadius + target.CollisionRadius)
		&& IsInFieldOfView(target);
}

bool Character::IsInViewDistance(const FVector& targetPos, const float viewDist)
{
	return _info.Pos.IsInRange(targetPos, viewDist);
}

bool Character::IsInFieldOfView(const FInfoData& target)
{
	float RadianYaw = _info.Yaw * (3.14159265f / 180.0f);
	FVector ForwardVector(std::cos(RadianYaw), std::sin(RadianYaw), 0.0f);
	FVector ToTarget = (target.Pos - _info.Pos).Normalize();
	float Dot = ForwardVector.DotProduct(ToTarget);
	float CosFOV = std::cos(_info.fovAngle * 0.5f * (3.14159265f / 180.0f));

	return Dot >= CosFOV;
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
	std::lock_guard<std::mutex> lock(_vlLock);
	if (!_viewList.contains(CharacterId))
	{
		_viewList.insert(CharacterId);
		return true;
	}
	return false;
}

bool Character::RemoveFromViewList(int32 CharacterId)
{
	std::lock_guard<std::mutex> lock(_vlLock);
	if (_viewList.contains(CharacterId))
	{
		_viewList.erase(CharacterId);
		return true;
	}
	return false;
}