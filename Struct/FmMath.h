#pragma once
#include "LastDropProcgen/GlobalHeader/QyEnum.h"

struct FmMath
{
	static float PerlinNoise2D(const FString& Seed, int GridTileSize, const FVector2D& MainPointLocation);
	static float PerlinNoise2D(const float Seed, int GridTileSize, const FVector2D& MainPointLocation);
	static float GetVertexDotProduct2D(const FString& GlobalSeed, const FVector2D& VertexLocation, const FVector2D& PointOffset);
	static FVector2D GetLocationGradient2D(const FString& GlobalSeed, const FVector2D& Location);
	static double GetSdCoef(ESdType InDeviationType);
	static void MutateSeed(int& SeedRef);
};
