#pragma once

struct FVector
{
	double X, Y, Z;

	FVector() : X(0), Y(0), Z(0) {}
	FVector(double x, double y, double z) : X(x), Y(y), Z(z) {}

	FVector operator-(const FVector& Other) const { return FVector(X - Other.X, Y - Other.Y, Z - Other.Z); }
	FVector operator+(const FVector& Other) const { return FVector(X + Other.X, Y + Other.Y, Z + Other.Z); }
	FVector operator*(float Scalar) const { return FVector(X * Scalar, Y * Scalar, Z * Scalar); }
	
	float Length() const { return std::sqrt(X * X + Y * Y + Z * Z); }
	FVector Normalize() const
	{
		float Len = Length();
		return (Len > 0) ? FVector(X / Len, Y / Len, Z / Len) : FVector(0, 0, 0);
	}

	double DistanceSquared(const FVector& Other) const
	{
		return (X - Other.X) * (X - Other.X) +
			(Y - Other.Y) * (Y - Other.Y) +
			(Z - Other.Z) * (Z - Other.Z);
	}

	bool IsInRange(const FVector& Other, double Range) const
	{
		return DistanceSquared(Other) <= (Range * Range);
	}
};
