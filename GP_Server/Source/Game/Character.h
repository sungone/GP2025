#pragma once

class Character
{
public:
	virtual void Init();
	virtual void Update() {};
	virtual bool IsDead();

	void OnDamaged(float damage);
	float OnAttacked()
	{
		return _info.GetAttackDamage();
	}
	void SetBunkerRandomLocation();
	FInfoData& GetInfo() { return _info; }
	bool IsValid() const { return _info.ID != -1; }
protected:
	std::mutex _cMutex;
	FInfoData _info;
	int32& _id = _info.ID;
};

