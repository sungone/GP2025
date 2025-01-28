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
	FInfoData& GetInfo() { return _info; }
	bool IsValid() const { return _info.ID != -1; }
protected:
	FInfoData _info;
	int32& _id = _info.ID;
};

