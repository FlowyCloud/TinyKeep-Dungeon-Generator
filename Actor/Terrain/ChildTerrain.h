// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChildTerrain.generated.h"

class AMasterTerrain;
class ATerrain;
class UProceduralMeshComponent;

UCLASS()
class LASTDROPPROCGEN_API AChildTerrain : public AActor
{
	GENERATED_BODY()

public:
	
	// OVERRIDE
	AChildTerrain();
	void Init(ATerrain* InTerrain, FVector InSize);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// COMPONENT
	UPROPERTY()
	UProceduralMeshComponent* ProceduralMesh;
	
	// VARIABLE
	UPROPERTY()
	AMasterTerrain* MasterTerrain;
	FIntPoint Size;
	FVector ParentLocation;

	// The lowest Z of procedural mesh
	float MinZ;
	
	// Local
	FVector ParentOffset;
	
	// FUNCTION
	void CreateMesh();
	void ReduceZBy(float InZ) const;

	//- Calculate
	float GetNoiseValue(FVector2D InVector) const;
};
