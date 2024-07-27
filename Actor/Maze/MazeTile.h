// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "LastDropProcgen/Struct/FmAxisRectangle.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeTile.generated.h"

class ADynamicMeshActor;
class AMaze;

UCLASS()
class LASTDROPPROCGEN_API AMazeTile : public AActor
{
	GENERATED_BODY()

	// Overrides
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Constructor
	AMazeTile();
	void Init(AMaze* InMaze, const FVector& InLocation, const FVector& InRoomDimension);

	// Variables from parent
	UPROPERTY()
	AMaze* MazeParent;
	FVector RoomDimension;

	// Variables
	FmAxisRectangle Rectangle;
	int PointQuantity;
	int Seed;

	// Arrays
	UPROPERTY()
	TArray <ADynamicMeshActor*> DynamicMeshActors;

	// Functions
	void ApplyMaterial(UMaterialInterface* InMaterial);

	void Clear();

	// Operators
	bool operator==(const AMazeTile& InTile) const
	{
		if (GetActorLocation() == InTile.GetActorLocation() && Rectangle == InTile.Rectangle)
			return true;
		return false;
	}
};
