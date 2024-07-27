#include "LastDropProcgen/Struct/FmPolygon.h"

FmPolygon::FmPolygon()
{
}

FmPolygon::FmPolygon(TArray<FmEdge>& EdgeArrayPara)
{
	for (int EdgeIndex = 0; EdgeIndex < EdgeArrayPara.Num(); EdgeIndex++)
	{
		Edges.Add(EdgeArrayPara[EdgeIndex]);
	}
}

// Add non duplicate edge to polygon
void FmPolygon::Add(FmEdge& EdgePara)
{
	bool NeedToAdd = true;
	for (int EdgeIndex = 0; EdgeIndex < Edges.Num(); EdgeIndex++)
	{
		if (EdgePara == Edges[EdgeIndex])
			NeedToAdd = false;
	}
	if (NeedToAdd)
		Edges.Add(EdgePara);
}

bool FmPolygon::IsContainEdge(FmEdge& EdgePara)
{
	for (int EdgeIndex = 0; EdgeIndex < Edges.Num(); EdgeIndex++)
	{
		if (EdgePara == Edges[EdgeIndex])
		{
			return true;
		}
	}

	return false;
}

bool FmPolygon::IsContainVertex(FVector2D& VertexPara)
{
	for (int EdgeIndex = 0; EdgeIndex < Edges.Num(); EdgeIndex++)
	{
		if (Edges[EdgeIndex].Vertices[0] == VertexPara || Edges[EdgeIndex].Vertices[1] == VertexPara)
		{
			return true;
		}
	}
	return false;
}

void FmPolygon::DrawPolygon(UWorld* World, FColor ColorPara, float ThicknessPara, float InZ)
{
	for (int EdgeIndex = 0; EdgeIndex < Edges.Num(); EdgeIndex++)
	{
		DrawDebugLine(World, 
			FVector(Edges[EdgeIndex].Vertices[0].X, Edges[EdgeIndex].Vertices[0].Y, InZ), 
			FVector(Edges[EdgeIndex].Vertices[1].X, Edges[EdgeIndex].Vertices[1].Y, InZ),
			ColorPara, true, 0.1f, 0, ThicknessPara);
	}
}

