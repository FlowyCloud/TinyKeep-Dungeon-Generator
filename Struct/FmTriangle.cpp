
#include "LastDropProcgen/Struct/FmTriangle.h"

FmTriangle::FmTriangle()
{
}

FmTriangle::FmTriangle(const FVector2D& Vertex1Para, const FVector2D& Vertex2Para, const FVector2D& Vertex3Para)
{
	A = Vertex1Para;
	B = Vertex2Para;
	C = Vertex3Para;
	Vertices = { A, B, C };
	FmEdge Edge1 = FmEdge(A, B);
	FmEdge Edge2 = FmEdge(B, C);
	FmEdge Edge3 = FmEdge(C, A);
	Edges = { Edge1, Edge2, Edge3 };

	CircumCenter = CalculateCircumcenter();
	CircumRadius = FVector2D::Distance(Vertices[0], CircumCenter);
	Centroid = CalculateCentroid();
}

FVector2D FmTriangle::CalculateCircumcenter()
{
	float D = (A.X * (B.Y - C.Y) + B.X * (C.Y - A.Y) + C.X * (A.Y - B.Y)) * 2;
	float X = ((A.X * A.X + A.Y * A.Y) * (B.Y - C.Y) + (B.X * B.X + B.Y * B.Y) * (C.Y - A.Y) + (C.X * C.X + C.Y * C.Y) * (A.Y - B.Y));
	float Y = ((A.X * A.X + A.Y * A.Y) * (C.X - B.X) + (B.X * B.X + B.Y * B.Y) * (A.X - C.X) + (C.X * C.X + C.Y * C.Y) * (B.X - A.X));

	return (D != 0.f ? FVector2D(X / D, Y / D) : FVector2D(0.f, 0.f));
}

FVector2D FmTriangle::CalculateCentroid()
{
	double cX = (A.X + B.X + C.X) / 3;
	double cY = (A.Y + B.Y + C.Y) / 3;
	return FVector2D(cX, cY);
}

void FmTriangle::DrawTriangle(UWorld* World, FColor ColorPara, float ThicknessPara, float InZ)
{
	DrawDebugLine(World, FVector(A.X, A.Y, InZ), FVector(B.X, B.Y, InZ), ColorPara, true, -1.0f, (uint8)0U, ThicknessPara);
	DrawDebugLine(World, FVector(B.X, B.Y, InZ), FVector(C.X, C.Y, InZ), ColorPara, true, -1.0f, (uint8)0U, ThicknessPara);
	DrawDebugLine(World, FVector(C.X, C.Y, InZ), FVector(A.X, A.Y, InZ), ColorPara, true, -1.0f, (uint8)0U, ThicknessPara);
}

void FmTriangle::DrawCirumcircle(UWorld* World, FColor ColorPara, float ThicknessPara, float InZ)
{
	DrawDebugSphere(World, FVector(CircumCenter.X, CircumCenter.Y, InZ), 20, 32, ColorPara, true);
	DrawDebugCircle(World, FVector(CircumCenter.X, CircumCenter.Y, InZ), CircumRadius, 256, ColorPara, true, -1.0f, (uint8)0U,
		ThicknessPara, FVector(0.f, 1.f, 0.f), FVector(1.f, 0.f, 0.f), false);
}

void FmTriangle::DrawStringAtCentroid(UWorld* World, FColor ColorPara, float InZ)
{
	DrawDebugString(World, FVector(Centroid.X, Centroid.Y, InZ), FString("Triangulation"), 0, ColorPara, 999);

}

bool FmTriangle::IsEqual(const FmTriangle& Triangle1Para, const FmTriangle& Triangle2Para)
{
	TArray<FVector2D> Triangle1Vertices = Triangle1Para.Vertices;
	TArray<FVector2D> Triangle2Vertices = Triangle2Para.Vertices;

	for (int VertexIndex1 = 0; VertexIndex1 < 3; VertexIndex1++)
	{
		for (int VertexIndex2 = 0; VertexIndex2 < Triangle2Vertices.Num(); VertexIndex2++)
		{
			if (Triangle1Vertices[VertexIndex1] == Triangle2Vertices[VertexIndex2])
			{
				Triangle2Vertices.RemoveAt(VertexIndex2);
				break;
			}
		}
	}
	if (Triangle2Vertices.Num() == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FmTriangle::IfEdgeIsShared(const FmEdge& EdgePara, const FmTriangle& TrianglePara)
{
	for (int TriangleEdgeIndex = 0; TriangleEdgeIndex < 3; TriangleEdgeIndex++)
	{
		if (EdgePara == TrianglePara.Edges[TriangleEdgeIndex])
		{
			return true;
		}
	}
	return false;
}

bool FmTriangle::IfEdgeIsNotShared(const FmEdge& EdgePara, const FmTriangle& TrianglePara)
{
	for (int TriangleEdgeIndex = 0; TriangleEdgeIndex < 3; TriangleEdgeIndex++)
	{
		if (EdgePara == TrianglePara.Edges[TriangleEdgeIndex])
		{
			return false;
		}
	}
	return true;
}

bool FmTriangle::HasInside(const FVector2D& VectorPara)
{
	if (Vertices.Contains(VectorPara))
	{
		return false;
	}
	bool IsInside = FVector2D::Distance(VectorPara, CircumCenter) < CircumRadius;
	return IsInside;
}

bool FmTriangle::HasAnyInside(const TArray<FVector2D>& VectorArrayPara)
{
	bool IsInside = false;
	for (int VectorIndex = 0; VectorIndex < VectorArrayPara.Num(); VectorIndex++)
	{
		IsInside = FVector2D::Distance(VectorArrayPara[VectorIndex], CircumCenter) < CircumRadius;
		if (IsInside == true)
		{
			break;
		}
	}
	return IsInside;
}

bool FmTriangle::IsSharedAnyVertex(FmTriangle TrianglePara1, FmTriangle TrianglePara2)
{
	for (int VertexIndex1 = 0; VertexIndex1 < 3; VertexIndex1++)
	{
		for (int VertexIndex2 = 0; VertexIndex2 < 3; VertexIndex2++)
		{
			if (TrianglePara1.Vertices[VertexIndex1] == TrianglePara2.Vertices[VertexIndex2])
			{
				return true;
			}
		}
	}
	return false;
}

