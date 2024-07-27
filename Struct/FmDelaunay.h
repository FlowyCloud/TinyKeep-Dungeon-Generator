#pragma once

#include "LastDropProcgen/Struct/FmTriangle.h"
#include "LastDropProcgen/Struct/FmPolygon.h"
#include "LastDropProcgen/Struct/FmGraph.h"

#include "FmDelaunay.generated.h"

USTRUCT()
struct FmDelaunay
{
	GENERATED_BODY()

	FmDelaunay();
	FmDelaunay(FVector2D& SpawnLocationPara, int QuantityPara, float MaxRangeXPara, float MaxRangeYPara, float MinRangeXPara, float MinRangeYPara, int InSeed = 1234);
	FmDelaunay(const TArray<FVector2D>& InPointArray, int InSeed = 1234);

	FmGraph CreateGraph();
	FmPolygon CreatePolygon();
	void BowyerWatson();
	void CreateSuperTriangle();
	void DrawSelf(UWorld* World, FColor InColor, float Thickness, float InZ);

	FVector2D SpawnLocation;
	int Quantity;
	float MaxRangeX;
	float MaxRangeY;
	float MinRangeX;
	float MinRangeY;
	FmTriangle SuperTriangle;
	int Seed;

	TArray<FVector2D> PointArray;
	TArray<FmTriangle> Triangulation;
	TArray<FmTriangle> TriangleToFormPolygon;

};