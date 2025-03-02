#include "pch.h"
#include "Character.h"

void Character::Init()
{
	SetParkingGarageRandomLocation();
	_info.InitStats(
		100.f,    // MaxHp
		20.f,     // Damage
		0.25f,    // CrtRate
		2.0f,     // CrtValue
		0.25f,    // Dodge
		200.f     // Speed
	);
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

void Character::SetBunkerRandomLocation()
{
	static constexpr std::pair<float, float> BUNKER_X_RANGE = { -3000.0f, -1000.0f };
	static constexpr std::pair<float, float> BUNKER_Y_RANGE = { -3500.0f, -1500.0f };
	static constexpr float BUNKER_Z = 116.0f;

	_info.SetLocation(
		RandomUtils::GetRandomFloat(BUNKER_X_RANGE.first, BUNKER_X_RANGE.second),
		RandomUtils::GetRandomFloat(BUNKER_Y_RANGE.first, BUNKER_Y_RANGE.second),
		BUNKER_Z
	);
}

void Character::SetParkingGarageRandomLocation()
{
	static constexpr std::pair<float, float> BUNKER_X_RANGE = { -2500.0f, 9000.0f };
	static constexpr std::pair<float, float> BUNKER_Y_RANGE = { -20000.0f, -10000.0f };
	static constexpr float BUNKER_Z = 150.0f;

	_info.SetLocation(
		RandomUtils::GetRandomFloat(BUNKER_X_RANGE.first, BUNKER_X_RANGE.second),
		RandomUtils::GetRandomFloat(BUNKER_Y_RANGE.first, BUNKER_Y_RANGE.second),
		BUNKER_Z
	);
}