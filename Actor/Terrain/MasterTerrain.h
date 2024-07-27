// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMeshActor.h"
#include "MasterTerrain.generated.h"

class ATerrain;

UCLASS()
class LASTDROPPROCGEN_API AMasterTerrain : public ADynamicMeshActor
{
	GENERATED_BODY()

public:
	AMasterTerrain();
	virtual void BeginPlay() override;

	// VARIABLE

	//- Child
	UPROPERTY()
	ATerrain* Terrain;
	
	//- Shape box
	UPROPERTY(EditAnywhere, Category = "CPP|Terrain")
	FIntPoint TerrainSize = FIntPoint(1000, 1000);
	UPROPERTY(EditAnywhere, Category = "CPP|ShapeBox")
	int ShapeBoxZ = 100;
	UPROPERTY(EditAnywhere, Category = "CPP|ShapeBox")
	UMaterial* ShapeBoxMaterial = nullptr;

	//- Terrain
	UPROPERTY(EditAnywhere, Category = "CPP|Terrain")
	bool IsTwoSided = true;
	UPROPERTY(EditAnywhere, Category = "CPP|Terrain")
	int ChunkSize = 1600;
	UPROPERTY(EditAnywhere, Category = "CPP|Terrain")
	int TileSize = 500;
	UPROPERTY(EditAnywhere, Category = "CPP|Terrain")
	int TerrainHeight = 5000;
	UPROPERTY(EditAnywhere, Category = "CPP|Terrain")
	int NoiseScale = 5000;
	UPROPERTY(EditAnywhere, Category = "CPP|Terrain")
	float UVScale = 1.0f;
	UPROPERTY(EditAnywhere, Category = "CPP|Terrain")
	UMaterialInterface* TerrainMaterial = nullptr;

	//- Detail
	UPROPERTY(EditAnywhere, Category = "CPP|TerrainDetail")
	int SpaceBetweenTerrainAndTheGround = 20;

	// FUNCTION
	UFUNCTION(CallInEditor, Category="CPP")
	void GenerateShape();
	UFUNCTION(CallInEditor, Category="CPP")
	void SpawnTerrain();
	UFUNCTION(CallInEditor, Category="CPP")
	void DeleteTerrain();
};
