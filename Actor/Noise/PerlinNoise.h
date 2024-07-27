// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMeshActor.h"
#include "DynamicTexture.h"
#include "UE5Coro/AsyncCoroutine.h"
#include "PerlinNoise.generated.h"



UCLASS()
class LASTDROPPROCGEN_API APerlinNoise : public ADynamicMeshActor
{
	GENERATED_BODY()
	
public:

	APerlinNoise();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, meta=(Latent, LatentInfo="Info"))
	FAsyncCoroutine ExampleLatent(FLatentActionInfo Info);
	bool MyFunction();
	void CheckMissingVariable() const;
	
	// VARIABLE
	FDynamicTexture DynamicTexture = FDynamicTexture();

	UPROPERTY(EditAnywhere, Category = "CPP")
	FString RandomSeed = FString(TEXT("Default"));
	UPROPERTY(EditAnywhere, Category = "CPP")
	FVector PlaneSize = FVector(1000, 1000, 100);
	UPROPERTY(EditAnywhere, Category = "CPP")
	float PerlinGridTileSize = 1;
	UPROPERTY(EditAnywhere, Category = "CPP")
	float PerlinResolution = 1;
	UPROPERTY(EditAnywhere, Category = "CPP")
	UCurveLinearColor* PerlinColorCurve = nullptr;
	
	// FUNCTION
	
	UFUNCTION(CallInEditor, Category="CPP")
	void CreatePlane();
	
	UFUNCTION(CallInEditor, Category="CPP")
	void GenerateTexture();
	UFUNCTION(BlueprintImplementableEvent, Category="CPP")
	void DisplayTexture(UTexture* Texture);
	
	void MakeTextureMap(TMap<FIntPoint, FLinearColor>& OutMap) const;
};
