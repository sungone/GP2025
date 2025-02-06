#pragma once

class CollisionManager
{
public:
    static CollisionManager& GetInst()
    {
        static CollisionManager inst;
        return inst;
    }

public:
    static bool CheckCollision(const FInfoData& A, const FInfoData& B)
    {
        float DistanceSq = A.Pos.DistanceSquared(B.Pos);
        float CollisionRange = A.CollisionRadius + B.CollisionRadius;

        return DistanceSq <= (CollisionRange * CollisionRange);
    }

    static bool CanAttack(const FInfoData& Attacker, const FInfoData& Target)
    {
        return Attacker.IsInAttackRange(Target);
    }

};
