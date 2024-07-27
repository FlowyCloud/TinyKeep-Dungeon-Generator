
#include "LastDropProcgen/Struct/FmEdge.h"
#include "LastDropProcgen/Struct/FmTriangle.h"

FmEdge::FmEdge()
{
}

FmEdge::FmEdge(FVector2D Vertex1Para, FVector2D Vertex2Para)
{
	Vertices = { Vertex1Para, Vertex2Para };
	Vertex1 = Vertex1Para;
	Vertex2 = Vertex2Para;
	Length = (Vertex1Para - Vertex2Para).Size();
}

bool FmEdge::operator==(const FmEdge& Other) const
{
	TArray<FVector2D> Edge1Vertices = Vertices;
	TArray<FVector2D> Edge2Vertices = Other.Vertices;

	for (int VertexIndex1 = 0; VertexIndex1 < 2; VertexIndex1++)
	{
		for (int VertexIndex2 = 0; VertexIndex2 < Edge2Vertices.Num(); VertexIndex2++)
		{
			if (Edge1Vertices[VertexIndex1] == Edge2Vertices[VertexIndex2])
			{
				Edge2Vertices.RemoveAt(VertexIndex2);
				break;
			}
		}
	}
	if (Edge2Vertices.Num() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FmEdge::IsContainPointFromTriangle(const FmTriangle& TrianglePara)
{
	for (int VertexIndex = 0; VertexIndex < 3; VertexIndex++)
	{
		if (Vertices[0] == TrianglePara.Vertices[VertexIndex] || Vertices[1] == TrianglePara.Vertices[VertexIndex])
		{
			return true;
		}
	}
	return false;
}

bool FmEdge::IsInArray(const TArray<FmEdge>& InArray)
{
	for (auto OtherEdge : InArray)
	{
		if (*this == OtherEdge)
			return true;
		else continue;
	}
	return false;
}

