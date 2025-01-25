#pragma once
#include "Define.h"

class Character
{
public:
	Character() { };

	virtual void Init();
	
	void OnDamaged(int damage);
	void SetBunkerRandomLocation();
	FInfoData& GetInfo() { return info; }

protected:
	FInfoData info;
	int& id = info.ID;
};

