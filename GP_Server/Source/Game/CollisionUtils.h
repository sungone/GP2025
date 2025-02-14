#pragma once
#include "Common.h"

namespace CollisionUtils
{
    inline bool CheckCollision(const FInfoData& A, const FInfoData& B)
    {
        float DistanceSq = A.Pos.DistanceSquared(B.Pos);
        float CollisionRange = A.CollisionRadius + B.CollisionRadius;

        return DistanceSq <= (CollisionRange * CollisionRange);
    }

    inline bool CanAttack(const FInfoData& Attacker, const FInfoData& Target)
    {
        return Attacker.IsInAttackRange(Target);
    }

};
