
// Must include
#include "LastDropProcgen/Actor/Maze/TestRoom.h"

#include "LastDropProcgen/Actor/Terrain/Terrain.h"

ATestRoom::ATestRoom()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATestRoom::BeginPlay()
{
	Super::BeginPlay();

	FVector Location;
	FVector BoxExtent;
	GetActorBounds(false, Location, BoxExtent);
	Location.Z = Location.Z + BoxExtent.Z + 100.f;
}


void ATestRoom::Tick(float DeltaTime)
{ 
	Super::Tick(DeltaTime);
}