#include "FmMath.h"

#include <random>

// Uniform distribution
float FmMath::PerlinNoise2D(const FString& Seed, const int GridTileSize, const FVector2D& MainPointLocation)
{ 
	const FVector2D BottomVertex(
		FMath::Floor(MainPointLocation.X / GridTileSize) * GridTileSize,
		FMath::Floor(MainPointLocation.Y / GridTileSize) * GridTileSize);
	const FVector2D MainPointOffsetRatio(
			(MainPointLocation.X - BottomVertex.X) / GridTileSize,
			(MainPointLocation.Y - BottomVertex.Y) / GridTileSize
		);

	float DotProduct1 = GetVertexDotProduct2D(Seed, FVector2D(BottomVertex.X, BottomVertex.Y), MainPointOffsetRatio);
	float DotProduct2 = GetVertexDotProduct2D(Seed, FVector2D(BottomVertex.X + GridTileSize, BottomVertex.Y), MainPointOffsetRatio);
	float DotProduct3 = GetVertexDotProduct2D(Seed, FVector2D(BottomVertex.X, BottomVertex.Y + GridTileSize), MainPointOffsetRatio);
	float DotProduct4 = GetVertexDotProduct2D(Seed, FVector2D(BottomVertex.X + GridTileSize, BottomVertex.Y + GridTileSize), MainPointOffsetRatio);
	
	const float ReturnValue = FMath::Lerp(
			FMath::Lerp(
				GetVertexDotProduct2D(Seed, FVector2D(BottomVertex.X, BottomVertex.Y), MainPointOffsetRatio),
				GetVertexDotProduct2D(Seed, FVector2D(BottomVertex.X + GridTileSize, BottomVertex.Y), MainPointOffsetRatio),
				MainPointOffsetRatio.X),
			FMath::Lerp(
				GetVertexDotProduct2D(Seed, FVector2D(BottomVertex.X, BottomVertex.Y + GridTileSize), MainPointOffsetRatio),
				GetVertexDotProduct2D(Seed, FVector2D(BottomVertex.X + GridTileSize, BottomVertex.Y + GridTileSize), MainPointOffsetRatio),
				MainPointOffsetRatio.X),
			MainPointOffsetRatio.Y
		);
	
	return ReturnValue;
}

float FmMath::PerlinNoise2D(const float Seed, const int GridTileSize, const FVector2D& MainPointLocation)
{
	return PerlinNoise2D(FString::SanitizeFloat(Seed), GridTileSize, MainPointLocation);
}

float FmMath::GetVertexDotProduct2D(const FString& GlobalSeed, const FVector2D& VertexLocation, const FVector2D& PointOffset)
{
	GetLocationGradient2D(GlobalSeed, VertexLocation);
	const FVector2D Gradient = GetLocationGradient2D(GlobalSeed, VertexLocation);
	const FVector2D Distance = PointOffset - VertexLocation;
	const float DotProduct = FVector2D::DotProduct(Gradient, Distance);
	
	return DotProduct;
}

// Combine the seed and the location to generate the output. Return a normalized vector
FVector2D FmMath::GetLocationGradient2D(const FString& GlobalSeed, const FVector2D& Location)
{
	const FString SeedString = FString::Format(TEXT("{0} and {1} and {2}"), {*GlobalSeed, Location.X, Location.Y});
	int SeedInt = GetTypeHash(SeedString);
	
	// Create generator
	std::mt19937 Generator(SeedInt);
	std::uniform_real_distribution<float> UniformDistribution{ -1.f, 1.f };
	
	// Make a random number
	const float RandomNumber1 = UniformDistribution(Generator);
	
	// Mutate the generator's seed and make a new random number
	MutateSeed(SeedInt);
	Generator.seed(SeedInt);
	const float RandomNumber2 = UniformDistribution(Generator);
	FVector2D OutVector(RandomNumber1, RandomNumber2);
	OutVector.Normalize();
	return OutVector;
}

double FmMath::GetSdCoef(ESdType InDeviationType)
{
	switch (InDeviationType)
	{
	case Percent_68:
		return 1.0 / 2.0;
	case Percent_95:
		return 1.0 / 4.0;
	case Percent_99:
		return 1.0 / 6.0;
	}
	return 0;
}

void FmMath::MutateSeed(int& SeedRef)
{
	SeedRef = (SeedRef * 196314165U) + 907633515U;
}
