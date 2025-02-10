#include "pch.h"
#include "Character.h"

void Character::Init()
{
	SetBunkerRandomLocation();
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

void Character::SetBunkerRandomLocation()
{
	static std::default_random_engine dre;
	static std::uniform_real_distribution<float> ud_x(-3000, -1000);
	static std::uniform_real_distribution<float> ud_y(-3500, -1500);
	static std::uniform_real_distribution<float> ud_yaw(-180.0f, 180.0f);

	_info.SetLocation(ud_x(dre), ud_y(dre), 116);
	_info.Yaw = ud_yaw(dre);
}