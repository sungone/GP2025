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

    bool IsInAttackRange(const FInfoData& target)
    {
        return _info.Pos.IsInRange(target.Pos, target.CollisionRadius + _info.AttackRange);
    }

    bool IsInViewDistance(const FVector& targetPos, float viewDist)
    {
        return _info.Pos.IsInRange(targetPos, viewDist);
    }

    bool IsInFieldOfView(const FInfoData& target, float fovAngle)
    {
        const float DegToRad = 3.14159265f / 180.0f;

        FVector toTarget = target.Pos - _info.Pos;
        float length = toTarget.Length();
        if (length > 0.0f)
        {
            toTarget = toTarget / length; // 정규화
        }
        else
        {
            return false; // 타겟과 같은 위치이면 시야 내에 없음
        }

        float yawRad = _info.Yaw * DegToRad;
        FVector forward(std::cos(yawRad), std::sin(yawRad), 0);

        float dotProduct = forward.DotProduct(toTarget);

        float cosFOV = std::cos(fovAngle * 0.5f * DegToRad);
        return dotProduct >= cosFOV;
    }

    bool HasLineOfSight(const FVector& targetPos, const std::vector<FVector>& obstacles)
    {
        for (const auto& obstacle : obstacles)
        {
            if (_info.Pos.IsInRange(obstacle, _info.Pos.DistanceSquared(targetPos) / 2))
            {
                return false;
            }
        }
        return true;
    }


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

