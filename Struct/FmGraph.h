#pragma once

#include "LastDropProcgen/Struct/FmEdge.h"

#include "FmGraph.generated.h"

USTRUCT()
struct FmGraph
{
	GENERATED_BODY()

	FmGraph() { Edges = {}; };
	FmGraph(const TArray<FmEdge>& EdgesPara);

	TArray<FmEdge> Edges;
	TArray<FVector2D> Vertices;

	void Add(FmEdge InEdge);
	FmGraph CreateMST(float InExtraRandomEdgePercentage = 0.f);
};


