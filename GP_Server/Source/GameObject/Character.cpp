#include "pch.h"
#include "Character.h"

void Character::Init()
{
	SetBunkerRandomLocation();
	_info.MaxHp = 100.f;
	_info.Hp = _info.MaxHp;
	_info.Damage = 50.f;
	_info.CrtRate = 0.25f;
	_info.CrtValue = 1.5f;
	_info.Dodge = 0.25f;
	_info.Speed = 200.f;
	_info.State = ECharacterStateType::STATE_IDLE;
}

bool Character::IsDead()
{
	return (_info.Hp <= 0);
}

void Character::OnDamaged(float damage)
{
	auto& hp = _info.Hp;
	if ((hp - damage) <= 0)
		hp = 0;
	else
	{
		hp -= damage;
	}
}

void Character::SetBunkerRandomLocation()
{
	static std::default_random_engine dre;
	static std::uniform_real_distribution<float> ud_x(-3000, -1000);
	static std::uniform_real_distribution<float> ud_y(-3500, -1500);

	_info.SetLocation(ud_x(dre), ud_y(dre), 116);
}