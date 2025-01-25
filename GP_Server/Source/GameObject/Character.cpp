#include "Character.h"

void Character::Init()
{
	SetBunkerRandomLocation();
	info.MaxHp = 100;
	info.Hp = info.MaxHp;
	info.Damage = 20;
}

void Character::OnDamaged(int damage)
{
	auto& hp = info.Hp;
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

	info.SetLocation(ud_x(dre), ud_y(dre), 116);
}