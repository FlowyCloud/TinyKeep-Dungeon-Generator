// Fill out your copyright notice in the Description page of Project Settings.

// Must include
#include "LastDropProcgen/Actor/Maze/MazeTile.h"

#include "DynamicMeshActor.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "LastDropProcgen/Actor/Maze/Maze.h"
#include "LastDropProcgen/Actor/Terrain/Terrain.h"
#include "LastDropProcgen/Struct/FmRandomStream.h"
#include "LastDropProcgen/Struct/FmAxisRectangle.h"
#include "LastDropProcgen/Struct/FmDelaunay.h"
#include "LastDropProcgen/Struct/FmPolygon.h"

AMazeTile::AMazeTile()
{
	PrimaryActorTick.bCanEverTick = false;
	if (!RootComponent)
	{
		SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root Component")));
	}
}

void AMazeTile::Init(AMaze* InMaze, const FVector& InLocation, const FVector& InRoomDimension)
{	
	MazeParent = InMaze;
	SetActorLocation(InLocation);
	RoomDimension = InRoomDimension;
	Rectangle = FmAxisRectangle(FVector2D(GetActorLocation().X - RoomDimension.X, GetActorLocation().Y - RoomDimension.Y),
		FVector2D(GetActorLocation().X + RoomDimension.X, GetActorLocation().Y + RoomDimension.Y));
	Seed = MazeParent->Seed;

	FmRandomStream UniformRS = FmRandomStream(Seed, EDistributionType::Uniform);
	FmRandomStream NormalRS = FmRandomStream(Seed, EDistributionType::Normal);

	//// Spawn the polygon's walls
	//PointQuantity = NormalRS.RandRange(10.f, 20.f);
	//FmDelaunay MyBowyerWatson = FmDelaunay(Location, PointQuantity, LengthX, LengthY, 1000.f, 1000.f, Seed);
	//FmPolygon MyPolygon = MyBowyerWatson.CreatePolygon();

	//for (FmEdge Edge : MyPolygon.Edges)
	//{
	//	FVector Vertex1 = Edge.Vertices[0];
	//	FVector Vertex2 = Edge.Vertices[1];
	//	FVector EdgeMiddleLocation = (Vertex1 + Vertex2) / 2;
	//	FRotator EdgeRotation = (Vertex2 - Vertex1).Rotation();
	//	FTransform MyTransform = FTransform(EdgeRotation, EdgeMiddleLocation);
	//	FVector ScaleVector = FVector((Vertex2 - Vertex1).Size(), 30, 600);

	//	AWall* Wall = GetWorld()->SpawnActorDeferred<AWall>(AWall::StaticClass(), MyTransform);
	//	Wall->Init(ScaleVector);
	//	Wall->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	//	Walls.Add(Wall);
	//}

	//// Spawn the rectangle's walls
	//for (FmEdge Edge : Rectangle.Edges)
	//{
	//	FVector Vertex1 = Edge.Vertices[0];
	//	FVector Vertex2 = Edge.Vertices[1];
	//	FRotator WallRotation = (Vertex2 - Vertex1).Rotation();
	//	FVector WallMiddleLocation = (Vertex1 + Vertex2) / 2;
	//	FVector WallScale = FVector((Vertex2 - Vertex1).Size(), 30, 600);

	//	AWall* Wall = GetWorld()->SpawnActorDeferred<AWall>(AWall::StaticClass(), FTransform(WallRotation, WallMiddleLocation));
	//	Wall->Init(WallScale);
	//	Wall->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	//	Walls.Add(Wall);
	//}

	// Spawn the ground
	ADynamicMeshActor* Room = GetWorld()->SpawnActorDeferred<ADynamicMeshActor>(ADynamicMeshActor::StaticClass(), FTransform(GetActorLocation()));
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendBox(Room->GetDynamicMeshComponent()->GetDynamicMesh(),
		FGeometryScriptPrimitiveOptions(), FTransform(), RoomDimension.X, RoomDimension.Y, RoomDimension.Z);
	Room->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	DynamicMeshActors.Add(Room);
}


void AMazeTile::BeginPlay()
{
	Super::BeginPlay();
}

void AMazeTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMazeTile::ApplyMaterial(UMaterialInterface* InMaterial)
{
	for (const ADynamicMeshActor* DynamicMeshActor : DynamicMeshActors)
	{
		DynamicMeshActor->GetDynamicMeshComponent()->SetMaterial(0, InMaterial);
	}
}

void AMazeTile::Clear()
{
	for (ADynamicMeshActor* DynamicMeshActor : DynamicMeshActors)
	{
		DynamicMeshActor->Destroy();
	}
	DynamicMeshActors.Empty();
	Destroy();
}
