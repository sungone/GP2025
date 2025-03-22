#include "pch.h"
#include "Character.h"
#include "NavMesh.h"

void Character::Init()
{
	_info.InitStats(
		100.f,    // MaxHp
		20.f,     // Damage
		0.25f,    // CrtRate
		2.0f,     // CrtValue
		0.25f,    // Dodge
		200.f     // Speed
	);
	_info.SetLocation(NavMesh::GetRandomPosition());
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

bool Character::IsInAttackRange(const FInfoData& target)
{
	return _info.Pos.IsInRange(target.Pos, target.CollisionRadius + _info.AttackRange);
}

bool Character::IsInViewDistance(const FVector& targetPos, float viewDist)
{
	return _info.Pos.IsInRange(targetPos, viewDist);
}

bool Character::IsInFieldOfView(const FInfoData& target, float fovAngle)
{
	const float DegToRad = 3.14159265f / 180.0f;

	FVector toTarget = target.Pos - _info.Pos;
	float length = toTarget.Length();
	if (length > 0.0f)
	{
		toTarget = toTarget / length; // 정규화
	}
	else
	{
		return false; // 타겟과 같은 위치이면 시야 내에 없음
	}

	float yawRad = _info.Yaw * DegToRad;
	FVector forward(std::cos(yawRad), std::sin(yawRad), 0);

	float dotProduct = forward.DotProduct(toTarget);

	float cosFOV = std::cos(fovAngle * 0.5f * DegToRad);
	return dotProduct >= cosFOV;
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
