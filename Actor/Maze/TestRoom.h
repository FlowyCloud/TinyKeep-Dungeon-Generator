#pragma once

// Must include
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestRoom.generated.h"

UCLASS()
class LASTDROPPROCGEN_API ATestRoom : public AActor
{
	GENERATED_BODY()
	
	ATestRoom();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
