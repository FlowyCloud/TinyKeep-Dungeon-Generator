#pragma once

#include "FmEdge.generated.h"

struct FmTriangle;

USTRUCT()
struct FmEdge
{
	GENERATED_BODY()

	FmEdge();
	FmEdge(FVector2D Vertex1Para, FVector2D Vertex2Para);

	bool IsContainPointFromTriangle(const FmTriangle& TrianglePara);
	bool IsInArray(const TArray<FmEdge>& InArray);
	bool operator==(const FmEdge& Other) const;

	float Length;
	TArray<FVector2D> Vertices;
	FVector2D Vertex1;
	FVector2D Vertex2;
};