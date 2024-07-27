// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LastDropProcgen/Struct/FmGraph.h"
#include "LastDropProcgen/Actor/Maze/MazeTile.h"
#include "LastDropProcgen/GlobalHeader/QyStruct.h"

#include "CoreMinimal.h"
#include "DynamicMeshActor.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "UE5Coro/AsyncCoroutine.h"
#include "Maze.generated.h"

UCLASS()
class LASTDROPPROCGEN_API AMaze : public ADynamicMeshActor
{
	GENERATED_BODY()
	
public:
	
	// OVERRIDE
	AMaze();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// UPROPERTY
	//- Maze
	UPROPERTY(EditAnywhere, Category = "CPP")
	bool IfNewSeed = false;
	UPROPERTY(EditAnywhere, Category = "CPP")
	float ForceStrength = 500.f;
	// This variable always have to be >= 1. If not, it will be set to 1.
	UPROPERTY(EditAnywhere, Category = "CPP")
	float ForceMaxRatio = 1.f;
	UPROPERTY(EditAnywhere, Category = "CPP")
	int NumberOfTile = 100;
	UPROPERTY(EditAnywhere, Category = "CPP")
	float SpecialTilePercentage = 1.25f;
	UPROPERTY(EditAnywhere, Category = "CPP")
	float XYZMinPercentage = 0.2;
	UPROPERTY(EditAnywhere, Category = "CPP")
	FVector2D EclipseDimension = FVector2D(1.f, 1.f);
	UPROPERTY(EditAnywhere, Category = "CPP")
	FVector MaxRoomDimension = FVector(1000.f, 1000.f, 500.f);
	
	//--Showcase
	UPROPERTY(EditAnywhere, Category = "CPP|Showcase")
	bool EnableShowcase = false;
	
	UPROPERTY(EditAnywhere, Category = "CPP|Showcase")
	bool EnableStep1 = false;
	UPROPERTY(EditAnywhere, Category = "CPP|Showcase")
	bool EnableStep2 = false;
	UPROPERTY(EditAnywhere, Category = "CPP|Showcase")
	bool EnableStep3 = false;
	UPROPERTY(EditAnywhere, Category = "CPP|Showcase")
	bool EnableStep4 = false;

	UPROPERTY(EditAnywhere, Category = "CPP|Showcase|Duration")
	float DelayBetweenSteps = 1.5f;
	UPROPERTY(EditAnywhere, Category = "CPP|Showcase|Duration")
	float Step1Duration = 5.f;
	UPROPERTY(EditAnywhere, Category = "CPP|Showcase|Duration")
	float Step2Period = 0.1f;
	UPROPERTY(EditAnywhere, Category = "CPP|Showcase|Duration")
	float Step3Duration = 10.f;
	UPROPERTY(EditAnywhere, Category = "CPP|Showcase|Duration")
	float Step4Duration = 5.f;

	//- Pathfinding
	UPROPERTY(EditAnywhere, Category = "CPP")
	FVector EndLocationVector = FVector(3000, 4000, 0);
	UPROPERTY(EditAnywhere, Category = "CPP")
	int PathfindingGridSize = 500;

	//- Material
	UPROPERTY(EditAnywhere, Category = "CPP")
	UMaterialInterface* SpecialTileMaterial;

	// Variables
	int Seed = 347234;
	float Z;
	FVector ShapeRatio = FVector();

	// Arrays
	UPROPERTY()
	TArray <AMazeTile*> Tiles = {};
	UPROPERTY()
	TArray <AMazeTile*> SpecialTiles = {};
	TArray <FmAxisRectangle> Rectangles = {};

	TArray <FVector> DebugPoints = {};
	TArray <DebugLine> DebugLines = {};
	TArray <DebugCircle> DebugCircles = {};

	FmGraph MazeSpanningTree = FmGraph();
	
	// FUNCTION
	UFUNCTION(CallInEditor, Category = "CPP")
	void SpawnMaze();
	UFUNCTION(CallInEditor, Category = "CPP")
	void ClearMaze();
	
	FAsyncCoroutine SpawnMaze_Implement();
	FAsyncCoroutine ApplySeparationForce();
	
	FVector CalculateSeparationForce(AMazeTile* InTile, const TArray<AMazeTile*>& InMazeTiles);
	FmGraph DoPathfinding(FVector InBeginPoint, FVector InEndPoint, float InGridSize, TArray<FmAxisRectangle> InObstacle);
	TArray<TSharedPtr<AStarPoint2D>> GetSurroundPoints(TSharedPtr<AStarPoint2D> InParent, TArray<FVector2D> InAxisVectors);
	TArray<FVector2D> GetAxisVectors(FVector2D InBeginPoint, FVector2D InEndPoint, float InRadius);
};