#pragma once

#include "LastDropProcgen/Struct/FmEdge.h"
#include "FmAxisRectangle.generated.h"

struct FmEdge;

USTRUCT()
struct FmAxisRectangle
{
	GENERATED_BODY()

	FmAxisRectangle();
	FmAxisRectangle(FVector2D MinPara, FVector2D MaxPara);

	float minX;
	float maxX;
	float minY;
	float maxY;
	float Area;
	float CircumRadius;
	FVector2D Center;
	TArray<FVector2D> Vertices;
	TArray<FmEdge> Edges;

	bool HasInside(FVector2D InVector);
	bool IsOverlappedWith(const FmAxisRectangle& RectanglePara);
	void DrawRectangle(UWorld* World, FColor InColor, float InThickness, float InZ);

	// define operator ==
	bool operator==(const FmAxisRectangle& RectanglePara) const
	{
		return (minX == RectanglePara.minX && maxX == RectanglePara.maxX && minY == RectanglePara.minY && maxY == RectanglePara.maxY);
	}
};
