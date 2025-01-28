#pragma once
#include "Define.h"

class Character
{
public:
	virtual void Init();
	virtual void Update() {};
	virtual bool IsDead();

	void OnDamaged(float damage);
	void SetBunkerRandomLocation();
	FInfoData& GetInfo() { return info; }
	bool IsValid() const { return info.ID != -1; }
protected:
	FInfoData info;
	int32& id = info.ID;
};

