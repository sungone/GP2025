#pragma once
#include "Common.h"

class Character
{
public:
	virtual void Init();
	virtual void Update() {};
	virtual bool IsDead();

	void OnDamaged(float damage);
	float GetAttackDamage();

    bool IsInAttackRange(const FInfoData& target);
    bool IsInViewDistance(const FVector& targetPos, float viewDist);
	bool IsInFieldOfView(const FInfoData& target, float fovAngle);
	bool HasLineOfSight(const FVector& targetPos, const std::vector<FVector>& obstacles);

	void SetBunkerRandomLocation();
	void SetParkingGarageRandomLocation();
	void SetTestRandomLocation();

	FInfoData& GetInfo() { return _info; }
	void SetInfo(FInfoData& info) { _info = info; }
	bool IsValid() const { return _info.ID != -1; }
protected:
	std::mutex _cMutex;
	FInfoData _info;
	int32& _id = _info.ID;
};

