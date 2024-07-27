#pragma once

struct DebugLine
{
	FVector A;
	FVector B;

	DebugLine(const FVector& InA, const FVector& InB)
	{
		A = InA;
		B = InB;
	}

	bool operator==(const DebugLine& Other) const { return A == Other.A && B == Other.B; }
};

struct DebugCircle
{
	FVector Center;
	float Radius;

	DebugCircle(const FVector& InCenter, const float InRadius)
	{
		Center = InCenter;
		Radius = InRadius;
	}
};

struct AStarPoint2D
{
	float X;
	float Y;
	FVector2D Vector2D;
	float CurrentF;

	TSharedPtr<AStarPoint2D> Parent;
	AStarPoint2D(float InX, float InY, TSharedPtr<AStarPoint2D> InParent = nullptr, float InCurrentF = 0.f)
	{
		X = InX;
		Y = InY;
		Vector2D = FVector2D(X, Y);
		Parent = InParent;
		CurrentF = InCurrentF;

	}
	AStarPoint2D(FVector2D InVector2D, TSharedPtr<AStarPoint2D> InParent = nullptr, float InCurrentF = 0.f)
	{
		X = InVector2D.X;
		Y = InVector2D.Y;
		Vector2D = FVector2D(X, Y);
		Parent = InParent;
		CurrentF = InCurrentF;
	}
	double GetDistance(FVector2D InVector2D) const { return FVector2D::Distance(FVector2D(X, Y), InVector2D); }
	bool IsInsideArray(const TArray<TSharedPtr<AStarPoint2D>>& InArray, float InMaxError) const
	{
		for (TSharedPtr<AStarPoint2D> Point : InArray)
		{
			if ( FMath::Abs(Point->X - X) < InMaxError && FMath::Abs(Point->Y - Y) < InMaxError)
				return true;
		}
		return false;
	}
	bool operator==(const AStarPoint2D& Other) const { return X == Other.X && Y == Other.Y; }
	bool operator!=(const AStarPoint2D& Other) const { return X != Other.X || Y != Other.Y; }
};