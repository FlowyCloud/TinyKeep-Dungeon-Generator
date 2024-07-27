
#include "LastDropProcgen/Struct/FmGraph.h"

#include "LastDropProcgen/Struct/FmRandomStream.h"

FmGraph::FmGraph(const TArray<FmEdge>& EdgesPara)
{
	for (const auto& Edge : EdgesPara)
	{
		if (!Edges.Contains(Edge))
			Edges.Add(Edge);
		if (!Vertices.Contains(Edge.Vertex1))
			Vertices.Add(Edge.Vertex1);
		if (!Vertices.Contains(Edge.Vertex2))
			Vertices.Add(Edge.Vertex2);
	}
}

struct GraphPoint;
struct GraphEdge;
struct Subset;

struct GraphPoint
{
	FVector2D Location;
	GraphPoint(FVector2D InLocation) { Location = InLocation; };
	TSharedPtr<Subset> ParentPointer = nullptr;
	bool operator==(const GraphPoint& Other) const { return Location == Other.Location; };
};


struct GraphEdge
{
	TArray<GraphPoint> Points = {};
	TArray<GraphPoint*> RealPoints = { nullptr, nullptr };
	float Length;
	GraphEdge(GraphPoint InPoint1, GraphPoint InPoint2) { Points.Add(InPoint1); Points.Add(InPoint2); Length = FVector2D::Distance(InPoint1.Location, InPoint2.Location); }
	bool operator==(const GraphEdge& Other) const { return (RealPoints == Other.RealPoints); }
};

struct Subset
{
	TArray<TSharedPtr<GraphEdge>> Edges = {};
	Subset(const TSharedPtr<GraphEdge>& InEdge) { Edges.Add(InEdge); }

	bool operator==(const Subset& Other) const { return Edges == Other.Edges; }
};

void FmGraph::Add(FmEdge InEdge)
{
	if (!Edges.Contains(InEdge))
		Edges.Add(InEdge);
	if (!Vertices.Contains(InEdge.Vertex1))
		Vertices.Add(InEdge.Vertex1);
	if (!Vertices.Contains(InEdge.Vertex2))
		Vertices.Add(InEdge.Vertex2);
}

FmGraph FmGraph::CreateMST(float InExtraRandomEdgePercentage)
{
	TArray <TSharedPtr<Subset>> ParentSet = {};
	TArray<TSharedPtr<GraphEdge>> GraphEdges = {};
	TArray<GraphPoint> GraphPoints = {};
	TArray<FmEdge> BadEdges = {};

	// Add all the edges in edges to begin edge array
	for (const FmEdge& Edge : Edges)
	{
		GraphEdges.Add(MakeShareable( new GraphEdge(GraphPoint(Edge.Vertex1), GraphPoint(Edge.Vertex2)) ));
	}

	// Add all the points in vertices to point list
	for (const FVector2D& Vertex : Vertices)
	{
		GraphPoints.Add(GraphPoint(Vertex));
	}

	// Set all the real point's pointers
	for (TSharedPtr<GraphEdge>& Edge : GraphEdges)
	{
		for (int PointInEdgeIndex = 0; PointInEdgeIndex < 2; PointInEdgeIndex++)
		{
			for (int PointInListIndex = 0; PointInListIndex < GraphPoints.Num(); PointInListIndex++)
			{
				if (Edge->Points[PointInEdgeIndex] == GraphPoints[PointInListIndex])
				{
					Edge->RealPoints[PointInEdgeIndex] = &GraphPoints[PointInListIndex];
					break;
				}
			}
		}
	}
	// Set all the edge's normal point to empty array
	for (TSharedPtr<GraphEdge>& Edge : GraphEdges)
	{
		Edge->Points.Empty();
	}
	GraphEdges.Sort([](const TSharedPtr<GraphEdge> A, const TSharedPtr<GraphEdge> B) { return A->Length < B->Length; });

	// Start of the big loop
	for (int i = 0; i < GraphEdges.Num(); i++)
	{
		const TSharedPtr<GraphEdge>& CurrentEdge = GraphEdges[i];

		TSharedPtr<Subset>& Point0Parent = CurrentEdge->RealPoints[0]->ParentPointer;
		TSharedPtr<Subset>& Point1Parent = CurrentEdge->RealPoints[1]->ParentPointer;

		// First case: If both point in edge have no parent
		if (Point0Parent == nullptr && Point1Parent == nullptr) 
		{
			TSharedPtr<Subset> SubsetPointer1(new Subset(CurrentEdge));
			ParentSet.Add(SubsetPointer1);
			Point0Parent = SubsetPointer1;
			Point1Parent = SubsetPointer1;
			continue;
		}

		// Second case: If the first point have parent and the second doesn't
		else if (Point0Parent != nullptr && Point1Parent == nullptr)
		{
			Point0Parent->Edges.Add(CurrentEdge);
			Point1Parent = Point0Parent;
			continue;
		}

		// Third case: If the first point doesn't have parent and the second does
		else if (Point0Parent == nullptr && Point1Parent != nullptr)
		{
			Point1Parent->Edges.Add(CurrentEdge);
			Point0Parent = Point1Parent;
			continue;
		}

		// Fourth case: If both point have parent
		else if (Point0Parent != nullptr && Point1Parent != nullptr)
		{
			// If they have the same parent
			if (Point0Parent == Point1Parent)
			{
				BadEdges.Add(FmEdge(CurrentEdge->RealPoints[0]->Location, CurrentEdge->RealPoints[1]->Location));
				continue;
			}
			// If they have different parent
			else
			{
				// Create and add a brand new subset
				TSharedPtr<Subset> SubsetPointer2(new Subset(CurrentEdge));
				ParentSet.Emplace(SubsetPointer2);
				ParentSet.Remove(Point0Parent);
				ParentSet.Remove(Point1Parent);

				TArray<TSharedPtr<GraphEdge>> EdgeChunk = {};
				EdgeChunk.Append(Point0Parent->Edges);
				EdgeChunk.Append(Point1Parent->Edges);

				// Set parent for all the point in the removed subsets
				// Include changing the current edge's point
				for (TSharedPtr<GraphEdge>& EdgeChunkPointer : EdgeChunk)
				{
					EdgeChunkPointer->RealPoints[0]->ParentPointer = SubsetPointer2;
					EdgeChunkPointer->RealPoints[1]->ParentPointer = SubsetPointer2;
				}
				SubsetPointer2->Edges.Append(EdgeChunk);


				//DrawDebugLine(World, CurrentEdge->RealPoints[0]->Location, CurrentEdge->RealPoints[1]->Location, FColor::Blue, true, -1.f, 0U, 15.f);
				continue;
			}
		}
	}

	//// Draw debug string at each point's location
	//for (const GraphPoint& Point : GraphPoints)
	//{
	//	for (int SubsetIndex = 0; SubsetIndex < ParentSet.Num(); SubsetIndex++)
	//	{
	//		if (Point.ParentPointer == ParentSet[SubsetIndex])
	//		{
	//			DrawDebugString(World, Point.Location, FString::FromInt(SubsetIndex), nullptr, FColor::Red);
	//			break;
	//		}
	//	}
	//}
	//int NumberOfSubset = ParentSet.Num();
	//UE_LOG(LogTemp, Warning, TEXT("Number of subset: %d"), NumberOfSubset);

	FmGraph GraphToReturn;
	if (ParentSet.Num() == 1)
	{
		for (const TSharedPtr<GraphEdge> CurrentEdge : ParentSet[0]->Edges)
		{
			GraphToReturn.Edges.Emplace(FmEdge((*CurrentEdge).RealPoints[0]->Location, (*CurrentEdge).RealPoints[1]->Location));
		}
		if (InExtraRandomEdgePercentage)
		{
			FmRandomStream UniformStream = FmRandomStream(ESeedType::UseTime, EDistributionType::Uniform);
			// Add extra random edges
			for (const FmEdge& BadEdge : BadEdges)
			{
				if (UniformStream.RandRange(0.f, 1.f) <= InExtraRandomEdgePercentage)
				{
					GraphToReturn.Edges.Emplace(BadEdge);
				}
			}
		}
		return GraphToReturn;
	}

	return FmGraph();
}