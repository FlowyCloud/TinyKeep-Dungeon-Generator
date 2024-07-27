// Fill out your copyright notice in the Description page of Project Settings.

// Must include
#include "LastDropProcgen/Actor/Maze/Maze.h"

#include "Editor.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LastDropProcgen/GlobalHeader/QyMacro.h"
#include "LastDropProcgen/Struct/FmRandomStream.h"
#include "LastDropProcgen/Struct/FmDelaunay.h"
#include "LastDropProcgen/Struct/FmEllipse.h"
#include "Tasks/Task.h"
#include "Tests/AutomationCommon.h"
#include "UE5Coro.h"


AMaze::AMaze()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void AMaze::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Reset actor scale so it works properly
	SetActorScale3D(FVector(1, 1, 1));
	
	DynamicMeshComponent->GetDynamicMesh()->Reset();
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendBox(DynamicMeshComponent->GetDynamicMesh(), FGeometryScriptPrimitiveOptions(),
		FTransform(), 100, 100, 100);
}

void AMaze::BeginPlay()
{
	Super::BeginPlay();

	//FVector Location;
	//FVector BoxExtent;
	//GetActorBounds(false, Location, BoxExtent);
	//Z = Location.Z + BoxExtent.Z + 200.f;
	//Location.Z = Z;
	// 
	//TArray<FmAxisRectangle> Obstacle = { FmAxisRectangle(FVector2D(3000.f, 3000.f), FVector2D(7000.f, 7000.f)) };
	//for (FmAxisRectangle Rect : Obstacle)
	//{
	//	Rect.DrawRectangle(GetWorld(), FColor::Red, 30.f, Z);
	//}
	//FmGraph MyPath = DoPathfinding(Location, Location + EndLocationVector, PathfindingGridSize, Obstacle);
	//MyPath.DrawGraph(GetWorld(), FColor::Red, 30.f, Z);
}

void AMaze::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	for (FVector DebugPoint : DebugPoints)
	{
		DrawDebugPoint(GetWorld(), DebugPoint, 10, FColor::Red, false, 0.f);
	}

	for(const DebugLine DebugLine : DebugLines)
	{
		DrawDebugLine(GetWorld(),
			DebugLine.A,
			DebugLine.B,
			FColor::Cyan, false, 0.f, 0U, 40.f);
	}
	
	for(const DebugCircle DebugCircle : DebugCircles)
	{
		DrawDebugCircle(GetWorld(), DebugCircle.Center, DebugCircle.Radius, 256, FColor::Red, false, 0.0f, 0,
				15.f, FVector(0.f, 1.f, 0.f), FVector(1.f, 0.f, 0.f), false);
	}

	if (!EnableShowcase || !EnableStep3)
	{
		for(const FmEdge Edge : MazeSpanningTree.Edges)
		{
			DrawDebugLine(GetWorld(),
				FVector(Edge.Vertex1.X, Edge.Vertex1.Y, Z),
				FVector(Edge.Vertex2.X, Edge.Vertex2.Y, Z),
				FColor::Cyan, false, 0.f, 0U, 40.f);
		}
	}
}

void AMaze::SpawnMaze()
{
	SpawnMaze_Implement();
}

FAsyncCoroutine AMaze::SpawnMaze_Implement()
{
	ClearMaze();
	// FVector Location;
	// FVector BoxExtent;
	// GetActorBounds(false, Location, BoxExtent);

	FVector Circumellipse = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
	float EclipseWidth = EclipseDimension.X * 200 * FMath::Sqrt(float(NumberOfTile / 100));
	float EclipseHeight = EclipseDimension.Y * 200 * FMath::Sqrt(float(NumberOfTile / 100));
	FmEllipse SmallCircle = FmEllipse(Circumellipse, EclipseWidth, EclipseHeight);

	if (EnableShowcase && EnableStep1)
	{
		co_await UE5Coro::Latent::Seconds(DelayBetweenSteps);
		// Draw the debug small circle
		DebugCircles.Emplace(DebugCircle(Circumellipse + FVector(0.f, 0.f, MaxRoomDimension.Z), EclipseWidth));
	}

	FmRandomStream NormalRS;
	FmRandomStream UniformRS;

	if (IfNewSeed)
	{
		UniformRS = FmRandomStream(ESeedType::UseTime, EDistributionType::Uniform);
		NormalRS = FmRandomStream(ESeedType::UseTime, EDistributionType::Normal);
	}
	else
	{
		UniformRS = FmRandomStream(Seed, EDistributionType::Uniform);
		NormalRS = FmRandomStream(Seed, EDistributionType::Normal);
	}

	if (ForceMaxRatio >= 1.f)
		ShapeRatio = FVector(UniformRS.RandRange(1.f, ForceMaxRatio), UniformRS.RandRange(1.f, ForceMaxRatio), 1.f);
	else
		ShapeRatio = FVector(1.f, 1.f, 1.f);
	
	// Very important. Delay nodes are limited by FPS. Default FPS is 120. In 5s there can only be 600 debug point spawned.
	// Can be improve by spawning more than 1 debug point each frame, but i am lazy.
	// Ok i will do it, but only so the end result would range between 5-10s. I can fix it in exactly 5s, but it will take some more
	// coding and i think it's overkill for such a simple showcase.
	const double Step1Period = Step1Duration / NumberOfTile;
	constexpr double FramePeriod = 1.0 / 120.0;
	
	int NumberOfPointEachFrame;
	if (Step1Period < FramePeriod)
		NumberOfPointEachFrame = FMath::FloorToInt(FramePeriod / Step1Period);
	else
		NumberOfPointEachFrame = 1;

	TArray <FVector> TileLocations = {};
	
	// Paint the debug points first
	if (EnableShowcase && EnableStep1)
		co_await UE5Coro::Latent::Seconds(DelayBetweenSteps);
	
	for (int i = 0; i < NumberOfTile; i++)
	{
		FVector TileLocation = SmallCircle.GenerateRandomPoint(UniformRS.GetSeed());
		TileLocations.Emplace(TileLocation);

		if (EnableShowcase && EnableStep1)
		{
			DebugPoints.Emplace(TileLocation + FVector(0.f, 0.f, MaxRoomDimension.Z));
			if ((i + 1) % NumberOfPointEachFrame == 0)
				co_await UE5Coro::Latent::Seconds(Step1Period);
		}

		//Rectangles.Add(MyTile->Rectangle);
		//MyTile->Rectangle.DrawRectangle(GetWorld(), FColor::Red, 40.f);
	}

	// Spawning new tiles as removing the debug points
	if (EnableShowcase && EnableStep1)
		co_await UE5Coro::Latent::Seconds(DelayBetweenSteps);
	
	for (int i = 0; i < TileLocations.Num(); i++)
	{
		FVector TileLocation = TileLocations[i];
		
		// Remove the debug point first
		if (EnableShowcase && EnableStep1)
			DebugPoints.Remove(TileLocation + FVector(0.f, 0.f, MaxRoomDimension.Z));

		float RoomX = NormalRS.RandRange(MaxRoomDimension.X * XYZMinPercentage, MaxRoomDimension.X);
		float RoomY = NormalRS.RandRange(MaxRoomDimension.Y * XYZMinPercentage, MaxRoomDimension.Y);
		float RoomZ = NormalRS.RandRange(MaxRoomDimension.Z * XYZMinPercentage, MaxRoomDimension.Z);
		FVector RoomDimension(RoomX, RoomY, RoomZ);
		
		AMazeTile* MyTile = GetWorld()->SpawnActorDeferred<AMazeTile>(AMazeTile::StaticClass(), FTransform(TileLocation));
		MyTile->Init(this, TileLocation, RoomDimension);
		MyTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		// I don't even remember why i have to set the location twice, but the code won't run if i didn't
		MyTile->SetActorLocation(TileLocation);
		Tiles.Add(MyTile);
		
		if (EnableShowcase && EnableStep1)
			if ((i + 1) % NumberOfPointEachFrame == 0)
				co_await UE5Coro::Latent::Seconds(Step1Period);
	}

	if (EnableShowcase && EnableStep1)
		DebugCircles.Empty();
	
	// Push the tiles away from each other
	co_await ApplySeparationForce();
	
	// Change all the big tile's material to special material
	float AverageArea = 0;
	for (AMazeTile* Tile : Tiles)
	{
		AverageArea += Tile->Rectangle.Area;
	}
	AverageArea /= Tiles.Num();
	
	for (AMazeTile* Tile : Tiles)
	{
		if (Tile->Rectangle.Area > AverageArea * SpecialTilePercentage)
		{
			SpecialTiles.Add(Tile);
			Tile->ApplyMaterial(SpecialTileMaterial);
		}
	}
	
	// Make Delaunay Triangulation out of the tiles
	TArray<FVector2D> TileLocations2D = {};
	for (AMazeTile* Tile : SpecialTiles)
	{
		TileLocations2D.Add(FVector2D(Tile->GetActorLocation().X, Tile->GetActorLocation().Y));
	}
	FmDelaunay MazeDelaunay = FmDelaunay(TileLocations2D, UniformRS.GetSeed());
	FmGraph MazeGraph = MazeDelaunay.CreateGraph();
	
	if (EnableShowcase && EnableStep3)
	{
		co_await UE5Coro::Latent::Seconds(DelayBetweenSteps);
		
		for (FmEdge Edge : MazeGraph.Edges)
		{
			DebugLines.Emplace(DebugLine(
				FVector(Edge.Vertex1.X, Edge.Vertex1.Y, Z),
				FVector(Edge.Vertex2.X, Edge.Vertex2.Y, Z)
				));
			co_await UE5Coro::Latent::Seconds(Step3Duration / 2 / MazeGraph.Edges.Num());
		}
	}
	
	MazeSpanningTree = MazeGraph.CreateMST(0.20f);
	if (EnableShowcase && EnableStep3)
	{
		co_await UE5Coro::Latent::Seconds(DelayBetweenSteps);
		
		for (FmEdge Edge : MazeGraph.Edges)
		{
			if (!MazeSpanningTree.Edges.Contains(Edge))
			{
				DebugLines.Remove(DebugLine(
				   FVector(Edge.Vertex1.X, Edge.Vertex1.Y, Z),
				   FVector(Edge.Vertex2.X, Edge.Vertex2.Y, Z)
				   ));
			}
			co_await UE5Coro::Latent::Seconds(Step3Duration / 2 / MazeGraph.Edges.Num());
		}
	}
}

void AMaze::ClearMaze()
{
	for (AMazeTile* MazeTile : Tiles)
	{
		MazeTile->Clear();
	}
	Tiles.Empty();
	SpecialTiles.Empty();
	
	DebugPoints.Empty();
	DebugLines.Empty();
	DebugCircles.Empty();

	MazeSpanningTree = FmGraph();
}


FAsyncCoroutine AMaze::ApplySeparationForce()
{
	if (EnableShowcase && EnableStep2)
		co_await UE5Coro::Latent::Seconds(DelayBetweenSteps);
	
	bool IsFinished = false;
	int a = 0;
	while (!IsFinished)
	{
		IsFinished = true;
		for (AMazeTile* Tile : Tiles)
		{
			FVector Force = CalculateSeparationForce(Tile, Tiles);
			if (Force != FVector::ZeroVector)
			{
				IsFinished = false;
				const FVector FinalForce = Force * ForceStrength * ShapeRatio;
				Tile->SetActorLocation(Tile->GetActorLocation() += FinalForce);
			}
		}
		if (EnableShowcase && EnableStep2)
		{
			co_await UE5Coro::Latent::Seconds(Step2Period);
		}

		a += 1;
		if (IsFinished)
		{
			break;
		}
	}
}

FVector AMaze::CalculateSeparationForce(AMazeTile* InTile, const TArray<AMazeTile*>& InMazeTiles)
{
	TArray <FVector> NeighborLocationArray;

	for (AMazeTile* OtherTile : InMazeTiles)
	{
		float Distance = FVector::Distance(InTile->GetActorLocation(), OtherTile->GetActorLocation());
		float MinDistance = InTile->Rectangle.CircumRadius + OtherTile->Rectangle.CircumRadius + 250.f;
		if (Distance != 0 && Distance < MinDistance)
		{
			NeighborLocationArray.Add(OtherTile->GetActorLocation());
		}
		// else continue;
	}

	if (NeighborLocationArray.Num() != 0)
	{
		FVector OutForce = FVector::ZeroVector;
		for (const FVector& NeighborLocation : NeighborLocationArray)
		{
			OutForce += (InTile->GetActorLocation() - NeighborLocation);
		}
		OutForce = OutForce.GetSafeNormal();
		return OutForce;
	}
	else
	{
		return FVector::ZeroVector;
	}
}


FmGraph AMaze::DoPathfinding(FVector InBeginPoint, FVector InEndPoint, float InGridSize, TArray<FmAxisRectangle> InObstacle)
{
	FmGraph ReturnGraph = FmGraph();
	float MinimumDistance = FMath::Sqrt(2 * InGridSize * InGridSize);

	FVector2D InBeginPoint2D = FVector2D(InBeginPoint);
	FVector2D InEndPoint2D = FVector2D(InEndPoint);

	TArray<TSharedPtr<AStarPoint2D>> OpenList = {};
	TArray<TSharedPtr<AStarPoint2D>> ClosedList = {};

	TSharedPtr<AStarPoint2D> BeginPoint = TSharedPtr<AStarPoint2D>(new AStarPoint2D(InBeginPoint.X, InBeginPoint.Y));
	OpenList.Emplace(BeginPoint);

	TSharedPtr<AStarPoint2D> LastNode = BeginPoint;
	while(OpenList.Num() > 0)
	//for (int j = 0; j < 20; j++)
	{
		TSharedPtr<AStarPoint2D> NodeWithLeastF = OpenList[0];
		int NodeIndex = 0;
		for (int i = 1; i < OpenList.Num(); i++)
		{
			if (OpenList[i]->CurrentF < NodeWithLeastF->CurrentF)
			{
				NodeWithLeastF = OpenList[i];
				NodeIndex = i;
			}
		}
		OpenList.RemoveAt(NodeIndex);
		ClosedList.Emplace(NodeWithLeastF);
		LastNode = NodeWithLeastF;

		// Break if reach target
		if (FVector2D::Distance(FVector2D(NodeWithLeastF->X, NodeWithLeastF->Y), InEndPoint2D) <= MinimumDistance)
		{
			ReturnGraph.Add(FmEdge
			(
				InEndPoint2D,
				FVector2D(NodeWithLeastF->X, NodeWithLeastF->Y)
			));
			TSharedPtr<AStarPoint2D> CurrentNode = NodeWithLeastF;
			while (CurrentNode != BeginPoint)
			{
				ReturnGraph.Add(FmEdge
				(
					FVector2D(CurrentNode->X, CurrentNode->Y),
					FVector2D(CurrentNode->Parent->X, CurrentNode->Parent->Y))
				);
				CurrentNode = CurrentNode->Parent;
			}
			break;
		}

		// Create surround points
		TArray<FVector2D> AxisVectors = GetAxisVectors(InBeginPoint2D, InEndPoint2D, PathfindingGridSize);
		TArray<TSharedPtr<AStarPoint2D>> SuroundPoints = GetSurroundPoints(NodeWithLeastF, AxisVectors);

		for (TSharedPtr<AStarPoint2D> Point : SuroundPoints)
		{
			if (Point->IsInsideArray(OpenList, float(PathfindingGridSize) / 100) || Point->IsInsideArray(ClosedList, float(PathfindingGridSize) / 100) || InObstacle[0].HasInside(Point->Vector2D))
			{
				continue;
			}
			else
			{
				Point->CurrentF = Point->GetDistance(InBeginPoint2D) + Point->GetDistance(InEndPoint2D);
				OpenList.Emplace(Point);
			}
		}
	}

	// Draw spheres to PIE
	DrawDebugSphere(GetWorld(), InEndPoint, 75.f, 16, FColor::Blue, true);
	for (TSharedPtr<AStarPoint2D> Point : OpenList)
	{
		DrawDebugSphere(GetWorld(), FVector(Point->X, Point->Y, Z), 50.f, 16, FColor::Black, true);
	}
	for (TSharedPtr<AStarPoint2D> Point : ClosedList)
	{
		DrawDebugSphere(GetWorld(), FVector(Point->X, Point->Y, Z), 75.f, 16, FColor::Red, true);
	}

	return ReturnGraph;
}

TArray<TSharedPtr<AStarPoint2D>> AMaze::GetSurroundPoints(TSharedPtr<AStarPoint2D> InParent, TArray<FVector2D> InAxisVectors)
{
	TArray<TSharedPtr<AStarPoint2D>> ReturnArray = {};
	
	FVector2D Bottom = FVector2D(InParent->Vector2D + (-InAxisVectors[0] - InAxisVectors[1]));
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i != 1 || j != 1)
				ReturnArray.Emplace(TSharedPtr<AStarPoint2D>(new AStarPoint2D
				(
					Bottom + (InAxisVectors[0] * i + InAxisVectors[1] * j),
					InParent
				)));
		}
	}
	return ReturnArray;
}

TArray<FVector2D> AMaze::GetAxisVectors(FVector2D InBeginPoint, FVector2D InEndPoint, float InRadius)
{
	float Slope = (InEndPoint.Y - InBeginPoint.Y) / (InEndPoint.X - InBeginPoint.X);
	float InverseSlope = -1 / Slope;

	float FirstVectorX = InRadius / FMath::Sqrt(Slope * Slope + 1);
	float SecondVectorX = InRadius / FMath::Sqrt(InverseSlope * InverseSlope + 1);
	FVector2D FirstVector = FVector2D(FirstVectorX, FirstVectorX * Slope);
	FVector2D SecondVector = FVector2D(SecondVectorX, SecondVectorX * InverseSlope);

	return { FirstVector, SecondVector };
}