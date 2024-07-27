// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterTerrain.h"

#include "Terrain.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"

class ATerrain;

AMasterTerrain::AMasterTerrain()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMasterTerrain::BeginPlay()
{
	Super::BeginPlay();

	GenerateShape();
	SpawnTerrain();
}

void AMasterTerrain::GenerateShape()
{
	// Reset actor scale so it works properly
	SetActorScale3D(FVector(1, 1, 1));
	
	DynamicMeshComponent->GetDynamicMesh()->Reset();
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendBox(DynamicMeshComponent->GetDynamicMesh(), FGeometryScriptPrimitiveOptions(),
	FTransform(), TerrainSize.X, TerrainSize.Y, ShapeBoxZ);
	DynamicMeshComponent->SetMaterial(0, ShapeBoxMaterial);
}

void AMasterTerrain::SpawnTerrain()
{
	GenerateShape();
	DeleteTerrain();
	
	const FTransform SpawningTransform(FVector(0, 0, ShapeBoxZ * 1));
	Terrain = GetWorld()->SpawnActorDeferred<ATerrain>(ATerrain::StaticClass(), SpawningTransform);
	Terrain->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	Terrain->Init(this);
	Terrain->FinishSpawning(SpawningTransform);
}

void AMasterTerrain::DeleteTerrain()
{
	GenerateShape();
	if (Terrain)
	{
		Terrain->DeleteChildTerrain();
		Terrain->Destroy();
		Terrain = nullptr;
	}
}

