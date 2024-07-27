#pragma once

#include "LastDropProcgen/Struct/FmEdge.h"
#include "FmTriangle.generated.h"

struct FmEdge;

USTRUCT()
struct FmTriangle
{
	GENERATED_BODY()

		FmTriangle();
	FmTriangle(const FVector2D& Vertex1Para, const FVector2D& Vertex2Para, const FVector2D& Vertex3Para);

	double CircumRadius;
	FVector2D CircumCenter;
	FVector2D Centroid;
	FVector2D A;
	FVector2D B;
	FVector2D C;

	TArray<FVector2D> Vertices;
	TArray<FmEdge> Edges;

	void DrawTriangle(UWorld* World, FColor ColorPara, float ThicknessPara, float InZ);
	void DrawCirumcircle(UWorld* World, FColor ColorPara, float ThicknessPara, float InZ);
	void DrawStringAtCentroid(UWorld* World, FColor ColorPara, float InZ);
	static bool IsEqual(const FmTriangle& Triangle1Para, const FmTriangle& Triangle2Para);
	static bool IfEdgeIsShared(const FmEdge& EdgePara, const FmTriangle& TrianglePara);
	static bool IfEdgeIsNotShared(const FmEdge& EdgePara, const FmTriangle& TrianglePara);
	bool HasInside(const FVector2D& VectorPara);
	bool HasAnyInside(const TArray<FVector2D>& VectorArrayPara);
	static bool IsSharedAnyVertex(FmTriangle TrianglePara1, FmTriangle TrianglePara2);
	FVector2D CalculateCentroid();
	FVector2D CalculateCircumcenter();
};
