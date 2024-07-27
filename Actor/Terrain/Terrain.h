// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LastDropProcgen/Actor/Terrain/ChildTerrain.h"

// Must include
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain.generated.h"

class AMasterTerrain;
class AMazeTile;
class UProceduralMeshComponent;

UCLASS()
class LASTDROPPROCGEN_API ATerrain : public AActor
{
	GENERATED_BODY()
	
public:

	// OVERRIDE
	ATerrain();
	void Init(AMasterTerrain* InMasterTerrain);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// VARIABLE

	UPROPERTY()
	AMasterTerrain* MasterTerrain;
	
	// Arrays
	UPROPERTY()
	TArray<AChildTerrain*> ChildTerrains;

	// FUNCTION
	void CreateMesh();
	void DeleteChildTerrain();
};
