// Fill out your copyright notice in the Description page of Project Settings.

// Must include
#include "LastDropProcgen/Actor/Terrain/ChildTerrain.h"

#include "LastDropProcgen/Actor/Terrain/Terrain.h"
#include "LastDropProcgen/Actor/Maze/Maze.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "MasterTerrain.h"

AChildTerrain::AChildTerrain()
{
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Procedural Mesh");
	ProceduralMesh->SetupAttachment(GetRootComponent());
}


void AChildTerrain::Init(ATerrain* InTerrain, FVector InSize)
{
	MasterTerrain = InTerrain->MasterTerrain;
	Size = FIntPoint(InSize.X, InSize.Y);
	ParentLocation = InTerrain->GetActorLocation();
	ParentOffset = GetActorLocation() - ParentLocation;

	CreateMesh();
}

void AChildTerrain::CreateMesh()
{
	// From master terrain
	const int TileSize = MasterTerrain->TileSize;
	const int TerrainHeight = MasterTerrain->TerrainHeight;
	const float UVScale = MasterTerrain->UVScale;
		
	const float Width = FMath::Abs(Size.X);
	const float Height = FMath::Abs(Size.Y);
	const int NumberOfRow = int(Width / TileSize + 1);
	const int NumberOfColumn = int(Height / TileSize + 1);

	// Create vertices array
	TArray<FVector> Vertices = {};
	TArray<FVector2D> UV0 = {};
	for (int i = 0; i < NumberOfRow - 1; i++)
	{
		const float CurrentX = Size.X >= 0 ? float(i * TileSize) : float(i * TileSize * -1);
		for (int j = 0; j < NumberOfColumn - 1; j++)
		{
			const float CurrentY = Size.Y >= 0 ? float(j * TileSize) : float(j * TileSize * -1);
			const float CurrentZ = float(TerrainHeight) * GetNoiseValue(FVector2D(CurrentX, CurrentY));
			Vertices.Emplace(FVector(CurrentX, CurrentY, CurrentZ));
			UV0.Emplace(FVector2D(CurrentX * UVScale, CurrentY * UVScale));
		}
		// For each row, add the last point that fit the border
		const float CurrentY = Size.Y;
		const float CurrentZ = float(TerrainHeight) * GetNoiseValue(FVector2D(CurrentX, CurrentY));
		Vertices.Emplace(FVector(CurrentX, CurrentY, CurrentZ));
		UV0.Emplace(FVector2D(CurrentX * UVScale, CurrentY * UVScale));
	}

	// Add the last row to the vertices array
	{
		const float CurrentX = Size.X;
		for (int i = 0; i < NumberOfColumn - 1; i++)
		{
			const float CurrentY = Size.Y >= 0 ? float(i * TileSize) : float(i * TileSize * -1);
			const float CurrentZ = float(TerrainHeight) * GetNoiseValue(FVector2D(CurrentX, CurrentY));
			Vertices.Emplace(FVector(CurrentX, CurrentY, CurrentZ));
			UV0.Emplace(FVector2D(Size.X * UVScale, CurrentY * UVScale));
		}
		// Add last point to the array
		const float CurrentY = Size.Y;
		const float CurrentZ = float(TerrainHeight) * GetNoiseValue(FVector2D(CurrentX, CurrentY));
		Vertices.Emplace(FVector(CurrentX, CurrentY, CurrentZ));
		UV0.Emplace(FVector2D(CurrentX * UVScale, CurrentY * UVScale));
	}
	
	// Find the lowest Z
	MinZ = MasterTerrain->TerrainHeight;
	for (const FVector Vertex : Vertices)
	{
		if (Vertex.Z < MinZ)
			MinZ = Vertex.Z;
	}

	// Create triangles array, order of the vertices matter
	TArray<int> Triangles = {};
	for (int i = 0; i < NumberOfRow - 1; i++)
	{
		for (int j = 0; j < NumberOfColumn - 1; j++)
		{
			int CurrentPoint = i * NumberOfColumn + j;
			Triangles.Emplace(CurrentPoint); // Current point
			Triangles.Emplace(CurrentPoint + 1); // Right point
			Triangles.Emplace(CurrentPoint + NumberOfColumn); // Top point

			Triangles.Emplace(CurrentPoint + NumberOfColumn + 1); // Top right point
			Triangles.Emplace(CurrentPoint + NumberOfColumn); // Top point
			Triangles.Emplace(CurrentPoint + 1); // Right point

			if (MasterTerrain->IsTwoSided)
			{
				Triangles.Emplace(CurrentPoint); // Current point
				Triangles.Emplace(CurrentPoint + NumberOfColumn); // Top point
				Triangles.Emplace(CurrentPoint + 1); // Right point

				Triangles.Emplace(CurrentPoint + NumberOfColumn + 1); // Top right point
				Triangles.Emplace(CurrentPoint + 1); // Right point
				Triangles.Emplace(CurrentPoint + NumberOfColumn); // Top point
			}
		}
	}

	TArray<FVector> Normals = {};
	TArray<FProcMeshTangent> Tangents = {};
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UV0, Normals, Tangents);

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, TArray<FColor>(), Tangents, true);
	ProceduralMesh->SetMaterial(0, MasterTerrain->TerrainMaterial);
}

void AChildTerrain::ReduceZBy(const float InZ) const
{
	// Second loop to minus that lowest Z to all Z in array
	ProceduralMesh->SetRelativeLocation(ProceduralMesh->GetRelativeLocation() - FVector(0, 0, InZ));
}

void AChildTerrain::BeginPlay()
{
	Super::BeginPlay();
}

void AChildTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AChildTerrain::GetNoiseValue(FVector2D InVector) const
{
	const float NoiseValue = FMath::PerlinNoise2D(FVector2D(
			(ParentOffset.X + InVector.X) / float(MasterTerrain->NoiseScale) + 0.1,
			(ParentOffset.Y + InVector.Y) / float(MasterTerrain->NoiseScale) + 0.1
		));
	// Add 0.75 so it's close to the ground. Idk why 1.0 won't fit
	return NoiseValue + 1;

	
}
