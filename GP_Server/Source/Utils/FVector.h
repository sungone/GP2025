#pragma once
#include <cmath>
#include <iostream>

struct FVector
{
    double X, Y, Z;

    FVector() : X(0), Y(0), Z(0) {}
    FVector(double x, double y, double z) : X(x), Y(y), Z(z) {}

    FVector operator-(const FVector& Other) const { return FVector(X - Other.X, Y - Other.Y, Z - Other.Z); }
    FVector operator+(const FVector& Other) const { return FVector(X + Other.X, Y + Other.Y, Z + Other.Z); }
    FVector operator*(float Scalar) const { return FVector(X * Scalar, Y * Scalar, Z * Scalar); }
    FVector operator/(float Scalar) const { return (Scalar != 0) ? FVector(X / Scalar, Y / Scalar, Z / Scalar) : FVector(0, 0, 0); }

    float Length() const { return std::sqrt(X * X + Y * Y + Z * Z); }

    FVector Normalize() const
    {
        float Len = Length();
        return (Len > 0) ? (*this) / Len : FVector(0, 0, 0);
    }

    float DistanceSquared(const FVector& Other) const
    {
        return (X - Other.X) * (X - Other.X) +
            (Y - Other.Y) * (Y - Other.Y) +
            (Z - Other.Z) * (Z - Other.Z);
    }

    bool IsInRange(const FVector& Other, float Range) const
    {
        return DistanceSquared(Other) <= (Range * Range);
    }

    float DotProduct(const FVector& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z;
    }

    float GetYawToTarget(const FVector& TargetPos) const
    {
        FVector Direction = (TargetPos - *this).Normalize();
        return std::atan2(Direction.Y, Direction.X) * (180.0f / 3.14159265f);
    }

    friend std::ostream& operator<<(std::ostream& os, const FVector& v)
    {
        os << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
        return os;
    }
};
