#include "pch.h"
#include "BoundingBox.h"

bool BoundingBox::CheckAABBCollision(const BoundingBox& Other) const
{
    return (Min.X <= Other.Max.X && Max.X >= Other.Min.X) &&
        (Min.Y <= Other.Max.Y && Max.Y >= Other.Min.Y) &&
        (Min.Z <= Other.Max.Z && Max.Z >= Other.Min.Z);
}