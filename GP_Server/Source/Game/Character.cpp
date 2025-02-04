#include "pch.h"
#include "Character.h"

void Character::Init()
{
	SetBunkerRandomLocation();
	_info.InitStats(
		100.f,    // MaxHp
		50.f,     // Damage
		0.25f,    // CrtRate
		1.5f,     // CrtValue
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
}

void Character::SetBunkerRandomLocation()
{
	static std::default_random_engine dre;
	static std::uniform_real_distribution<float> ud_x(-3000, -1000);
	static std::uniform_real_distribution<float> ud_y(-3500, -1500);

	_info.SetLocation(ud_x(dre), ud_y(dre), 116);
}