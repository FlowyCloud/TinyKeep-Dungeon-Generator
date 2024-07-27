// Fill out your copyright notice in the Description page of Project Settings.

// Must include
#include "LastDropProcgen/Actor/Terrain/Terrain.h"

#include "LastDropProcgen/Actor/Maze/MazeTile.h"
#include "LastDropProcgen/Actor/Maze/Maze.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "MasterTerrain.h"

ATerrain::ATerrain()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void ATerrain::Init(AMasterTerrain* InMasterTerrain)
{
	MasterTerrain = InMasterTerrain;
	CreateMesh();
}

void ATerrain::BeginPlay()
{
	Super::BeginPlay();
	CreateMesh();
}

void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATerrain::CreateMesh()
{
	DeleteChildTerrain();
	
	//
	const FIntPoint Size = MasterTerrain->TerrainSize;
	const FVector BeginPoint = FVector(-Size.X / 2, -Size.Y / 2, 0);
	const FVector EndPoint = FVector(Size.X / 2, Size.Y / 2, 0);

	const int ChunkSize = MasterTerrain->ChunkSize;
	const int XDimension = int(Size.X / ChunkSize) + 1;
	const int YDimension = int(Size.Y / ChunkSize) + 1;

	// Create vertices array
	TArray<TArray<FVector>> Vertices = {};
	for (int i = 0; i < XDimension + 1; i++)
	{
		Vertices.Emplace(TArray<FVector>());
		for (int j = 0; j < YDimension + 1; j++)
		{
			Vertices[i].Emplace(FVector());
		}
	}

	// Add item to vertices array
	for (int i = 0; i < XDimension; i++)
	{
		// Column -1 because i add last point outside of for loop
		const float CurrentX = BeginPoint.X + i * ChunkSize;
		for (int j = 0; j < YDimension; j++)
		{
			const float CurrentY = BeginPoint.Y + j * ChunkSize;
			Vertices[i][j] = FVector(CurrentX, CurrentY, 0);
		}
		// Add last point in each column
		const float CurrentY = EndPoint.Y;
		Vertices[i][YDimension] = FVector(CurrentX, CurrentY, 0);
	}
	
	// Add last column 
	const float CurrentX = EndPoint.X;
	for (int i = 0; i < YDimension; i++)
	{
		const float CurrentY = BeginPoint.Y + i * ChunkSize;
		Vertices[XDimension][i] = FVector(CurrentX, CurrentY, 0);
	}
	// Add last point in last column
	const float CurrentY = EndPoint.Y;
	Vertices[XDimension][YDimension] = FVector(CurrentX, CurrentY, 0);

	// Spawn child terrains, order of the vertices matter
	for (int i = 0; i < XDimension; i++)
	{
		for (int j = 0; j < YDimension; j++)
		{
			const FVector BeginLocation = Vertices[i][j];
			const FVector EndLocation = Vertices[i + 1][j + 1];
			const FTransform SpawnTransform = FTransform(BeginLocation);
			
			AChildTerrain* MyChildTerrain = GetWorld()->SpawnActorDeferred<AChildTerrain>(AChildTerrain::StaticClass(), SpawnTransform);
			MyChildTerrain->Init(this, EndLocation - BeginLocation);
			MyChildTerrain->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			MyChildTerrain->FinishSpawning(SpawnTransform);
			
			ChildTerrains.Emplace(MyChildTerrain);
		}
	}
	
	float TempMinZ = MasterTerrain->TerrainHeight;
	for (const AChildTerrain* ChildTerrain : ChildTerrains)
	{
		if (ChildTerrain->MinZ < TempMinZ)
		{
			TempMinZ = ChildTerrain->MinZ;
		}
	}
	for (const AChildTerrain* ChildTerrain : ChildTerrains)
	{
		ChildTerrain->ReduceZBy(TempMinZ - MasterTerrain->SpaceBetweenTerrainAndTheGround);
	}
}

void ATerrain::DeleteChildTerrain()
{
	for (AChildTerrain* ChildTerrain : ChildTerrains)
	{
		ChildTerrain->Destroy();
	}
	ChildTerrains.Empty();
}

