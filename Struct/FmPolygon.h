#pragma once

#include "LastDropProcgen/Struct/FmEdge.h"
#include "FmPolygon.generated.h"


USTRUCT()
struct FmPolygon
{
	GENERATED_BODY()

	FmPolygon();
	FmPolygon(TArray<FmEdge>& EdgeArrayPara);

	TArray<FmEdge> Edges;

	void Add(FmEdge& EdgePara);
	bool IsContainEdge(FmEdge& EdgePara);
	bool IsContainVertex(FVector2D& VertexPara);
	void DrawPolygon(UWorld* World, FColor ColorPara, float ThicknessPara, float InZ);
};
