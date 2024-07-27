
#include "LastDropProcgen/Struct/FmDelaunay.h"

#include "LastDropProcgen/Struct/FmRandomStream.h"
#include "LastDropProcgen/Struct/FmEdge.h"

FmDelaunay::FmDelaunay()
{
}

// Input parameters are: center spawn location, numbers of points, half the length of outside rectangle, half the length of inside rectangle, seed
FmDelaunay::FmDelaunay(FVector2D& SpawnLocationPara, int QuantityPara, float MaxRangeXPara, float MaxRangeYPara, float MinRangeXPara, float MinRangeYPara, int InSeed)
{
	SpawnLocation = SpawnLocationPara;
	Quantity = QuantityPara;
	MinRangeX = MinRangeXPara;
	MinRangeY = MinRangeYPara;
	MaxRangeX = MaxRangeXPara;
	MaxRangeY = MaxRangeYPara;
	Seed = InSeed;
	
	FmRandomStream UniformRS = FmRandomStream(Seed, EDistributionType::Uniform);
	for (int i = 0; i < Quantity; i++)
	{
		double PointLocationX = SpawnLocation.X + UniformRS.RandRange(-MaxRangeX, MaxRangeX);
		double PointLocationY = SpawnLocation.Y + UniformRS.RandRange(-MaxRangeY, MaxRangeY);
		PointArray.Add(FVector2D(PointLocationX, PointLocationY));
	}
	BowyerWatson();
}

FmDelaunay::FmDelaunay(const TArray<FVector2D>& InPointArray, int InSeed)
{
	PointArray = InPointArray;
	Seed = InSeed;
	BowyerWatson();
}

FmGraph FmDelaunay::CreateGraph()
{
	TArray <FmEdge> EdgeArray = {};

	for (auto& Triangle : Triangulation)
	{
		for (auto& Edge : Triangle.Edges)
		{
			if (!Edge.IsInArray(EdgeArray))
				EdgeArray.Add(Edge);
		}
	}
	return FmGraph(EdgeArray);
}

FmPolygon FmDelaunay::CreatePolygon()
{
	FmPolygon PolygonVar = FmPolygon();
	for (auto& Triangle : TriangleToFormPolygon)
	{
		for (auto& Edge : Triangle.Edges)
		{
			if (!Edge.IsContainPointFromTriangle(SuperTriangle))
				PolygonVar.Add(Edge);
		}
	}
	return PolygonVar;
}

void FmDelaunay::BowyerWatson()
{
	// Add super-triangle to triangulation
	CreateSuperTriangle();
	Triangulation.Add(SuperTriangle);

	// Triangulation = empty triangle mesh data structure
	TArray<FmTriangle> BadTriangles;
	FmPolygon Polygon;

	TArray<int> DotIndexes = {};
	for (int i = 0; i < PointArray.Num(); i++)
	{
		DotIndexes.Add(i);
	}

	// For each point in pointList do
	for (int CurrentDotIndexes: DotIndexes)
		/*for (int CurrentIndex = 0; CurrentIndex < PointNum; CurrentIndex++)*/
	{
		FVector2D CurrentPoint = PointArray[CurrentDotIndexes];
		BadTriangles = {};
		Polygon.Edges = {};

		// For each triangle in triangulation do
		for (int TriangleIndex = 0; TriangleIndex < Triangulation.Num(); TriangleIndex++)
		{
			// If point is inside circumcircle of triangle
			if (Triangulation[TriangleIndex].HasInside(CurrentPoint))
			{
				// Add triangle to badTriangles
				BadTriangles.Add(Triangulation[TriangleIndex]);
			}
		}

		// If there is only 1 triangle in badTriangles
		if (BadTriangles.Num() == 1)
		{
			for (int EdgeIndex = 0; EdgeIndex < 3; EdgeIndex++)
			{
				FmEdge CurrentEdge = BadTriangles[0].Edges[EdgeIndex];
				Polygon.Add(BadTriangles[0].Edges[EdgeIndex]);
			}
		}
		else
		{
			// For each BadTriangle in BadTriangles do
			for (int BadTriangleIndex = 0; BadTriangleIndex < BadTriangles.Num(); BadTriangleIndex++)
			{
				FmPolygon BadPolygon;
				// Loop through all the other bad triangle
				for (int OtherBadTriangleIndex = 0; OtherBadTriangleIndex < BadTriangles.Num(); OtherBadTriangleIndex++)
				{
					if (OtherBadTriangleIndex != BadTriangleIndex)
					{
						// Loop through the bad triangle's edges
						for (int OtherBadTriangleEdgeIndex = 0; OtherBadTriangleEdgeIndex < 3; OtherBadTriangleEdgeIndex++)
						{
							BadPolygon.Add(BadTriangles[OtherBadTriangleIndex].Edges[OtherBadTriangleEdgeIndex]);
						}
					}
				}
				// For each edge in BadTriangle do
				for (int EdgeIndex = 0; EdgeIndex < 3; EdgeIndex++)
				{
					FmEdge CurrentEdge = BadTriangles[BadTriangleIndex].Edges[EdgeIndex];
					// If edge is not shared by any other triangles in badTriangles
					if (!BadPolygon.IsContainEdge(CurrentEdge))
					{
						Polygon.Add(CurrentEdge);
					}
				}
			}
		}

		// For each triangle in badTriangles do
		for (int BadTriangleIndex = 0; BadTriangleIndex < BadTriangles.Num(); BadTriangleIndex++)
		{
			for (int TriangulationIndex = 0; TriangulationIndex < Triangulation.Num(); TriangulationIndex++)
			{
				if (FmTriangle::IsEqual(BadTriangles[BadTriangleIndex], Triangulation[TriangulationIndex]))
				{
					// Remove triangle from triangulation
					Triangulation.RemoveAt(TriangulationIndex);
				}
			}
		}

		// For each edge in polygon do
		for (int EdgeIndex = 0; EdgeIndex < Polygon.Edges.Num(); EdgeIndex++)
		{
			// NewTri = form a triangle from edge to point
			FmTriangle NewTri = FmTriangle(Polygon.Edges[EdgeIndex].Vertices[0], Polygon.Edges[EdgeIndex].Vertices[1], CurrentPoint);
			// Add newTri to triangulation
			Triangulation.Add(NewTri);
		}
	}


	// For each triangle in triangulation
	for (int TriangulationIndex = 0; TriangulationIndex < Triangulation.Num(); TriangulationIndex++)
	{
		FmTriangle CurrentTriangle = Triangulation[TriangulationIndex];
		// If triangle contains a vertex from original super-triangle
		if (FmTriangle::IsSharedAnyVertex(CurrentTriangle, SuperTriangle))
		{
			// Remove triangle from triangulation
			TriangleToFormPolygon.Add(Triangulation[TriangulationIndex]);
			Triangulation.RemoveAt(TriangulationIndex);
			TriangulationIndex--;
		}
	}
}

void FmDelaunay::CreateSuperTriangle()
{
	// Find the min and max of x and y axis to know the rectangle around points
	double MinX = PointArray[0].X;
	double MaxX = PointArray[0].X;
	double MinY = PointArray[0].Y;
	double MaxY = PointArray[0].Y;
	for (int i = 1; i < PointArray.Num(); i++)
	{
		if (PointArray[i].X < MinX)
		{
			MinX = PointArray[i].X;
		}
	}
	for (int i = 1; i < PointArray.Num(); i++)
	{
		if (PointArray[i].X > MaxX)
		{
			MaxX = PointArray[i].X;
		}
	}
	for (int i = 1; i < PointArray.Num(); i++)
	{
		if (PointArray[i].Y < MinY)
		{
			MinY = PointArray[i].Y;
		}
	}
	for (int i = 1; i < PointArray.Num(); i++)
	{
		if (PointArray[i].Y > MaxY)
		{
			MaxY = PointArray[i].Y;
		}
	}

	// Calculate the circle based on the rectangle
	double CircleRadius = FVector2D::Distance(FVector2D(MinX, MinY), FVector2D(MaxX, MaxY)) / 2;
	FVector2D CircleCenter = FVector2D(MinX + (MaxX - MinX) / 2, MinY + (MaxY - MinY) / 2);

	// Calculate 3 triangle vertex based on the circle
	double BigCircleRatio = 50;
	FVector2D PointA = FVector2D(CircleCenter.X, CircleCenter.Y + CircleRadius * BigCircleRatio);
	FVector2D PointB = FVector2D(CircleCenter.X - (FMath::Sin(FMath::DegreesToRadians(60.0)) * CircleRadius * BigCircleRatio), CircleCenter.Y - (FMath::Cos(FMath::DegreesToRadians(60.0)) * CircleRadius * BigCircleRatio));
	FVector2D PointC = FVector2D(CircleCenter.X + (FMath::Sin(FMath::DegreesToRadians(60.0)) * CircleRadius * BigCircleRatio), CircleCenter.Y - (FMath::Cos(FMath::DegreesToRadians(60.0)) * CircleRadius * BigCircleRatio));

	FmTriangle SuperTriangleVar = FmTriangle(PointA, PointB, PointC);
	SuperTriangle = SuperTriangleVar;
}

void FmDelaunay::DrawSelf(UWorld* World, FColor InColor, float Thickness, float InZ)
{
	for (int TriangleIndex = 0; TriangleIndex < Triangulation.Num(); TriangleIndex++)
	{
		Triangulation[TriangleIndex].DrawTriangle(World, InColor, Thickness, InZ);
	}
}
