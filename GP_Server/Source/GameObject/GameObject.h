#pragma once
#include "Define.h"

class GameObject
{
public:
	GameObject() {};
	void SetBunkerRandomLocation()
	{
		static std::default_random_engine dre;
		static std::uniform_real_distribution<float> ud_x(-3000, -1000);
		static std::uniform_real_distribution<float> ud_y(-3500, -1500);

		info.SetLocation(ud_x(dre), ud_y(dre), 116);
	}
	virtual void Init()
	{
		info.MaxHp = 100.0f;
		info.Hp = info.MaxHp;
		SetBunkerRandomLocation();
	}
	FInfoData GetInfo() const { return info; }
protected:
	FInfoData info;
	int& id = info.ID;
};

