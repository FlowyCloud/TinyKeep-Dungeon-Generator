// Fill out your copyright notice in the Description page of Project Settings.

// Must include
#include "LastDropProcgen/Actor/Noise//PerlinNoise.h"

#include <functional>
#include "Curves/CurveLinearColor.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "LastDropProcgen/GlobalHeader/QyMacro.h"
#include "LastDropProcgen/Multithreading/MyThread.h"
#include "LastDropProcgen/Struct/FmMath.h"
#include "LastDropProcgen/Struct/FmRandomStream.h"
#include "Tasks/Task.h"
#include "UE5Coro.h"

DECLARE_CYCLE_STAT(TEXT("GenerateTexture"), STAT_GenerateTexture, STATGROUP_ProcGen);

APerlinNoise::APerlinNoise()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APerlinNoise::BeginPlay()
{
	Super::BeginPlay();
	
	CheckMissingVariable();
}

FAsyncCoroutine APerlinNoise::ExampleLatent(FLatentActionInfo Info)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "Before co_await");
	co_await UE5Coro::Latent::Seconds(2.0f);
	co_await UE5Coro::Latent::Until(std::bind(&APerlinNoise::MyFunction, this));
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "After co_await");
}

bool APerlinNoise::MyFunction()
{
	return true;
}

void APerlinNoise::CheckMissingVariable() const
{
	if (!PerlinColorCurve)
	{
		PRINT(Red, "APerlinNoise: Missing PerlinColorCurve")
	}
}

void APerlinNoise::CreatePlane()
{
	// Reset actor scale so it works properly
	SetActorScale3D(FVector(1, 1, 1));
	
	DynamicMeshComponent->GetDynamicMesh()->Reset();
	UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendRectangleXY(DynamicMeshComponent->GetDynamicMesh(), FGeometryScriptPrimitiveOptions(),
		FTransform(), PlaneSize.X, PlaneSize.Y, PlaneSize.Z);
}

void APerlinNoise::GenerateTexture()
{ 
	// co_await UE5Coro::Async::MoveToThread(ENamedThreads::AnyBackgroundThreadNormalTask);
	// TMap<FIntPoint, FLinearColor> MyMap;
	// MakeTextureMap(MyMap);
	// co_await UE5Coro::Async::MoveToGameThread();
	// DynamicTexture = FDynamicTexture(FIntPoint(PlaneSize.X, PlaneSize.Y), FLinearColor::Red);
	// DynamicTexture.SetTextureFromData(MyMap);
	// DisplayTexture();
	// ActualPrint();
	
	// using namespace UE::Tasks;
	// TMap<FIntPoint, FLinearColor> MyMap;	
	// TTask<void> TaskA = Launch(UE_SOURCE_LOCATION, [&]()
	// 	{
	// 		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&] ()
	// 			{
	// 				MakeTextureMap(MyMap);
	// 			});
	// 	});
	// TTask<void> TaskB = Launch(UE_SOURCE_LOCATION,
	// 	[&] () mutable
	// 	{
	// 		DynamicTexture = FDynamicTexture(FIntPoint(PlaneSize.X, PlaneSize.Y), FLinearColor::White);
	// 		DynamicTexture.SetTextureFromData(MyMap);
	// 		DisplayTexture();
	// 	},
	// 	TaskA);
	

	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [&] ()
	{
		TMap<FIntPoint, FLinearColor> MyMap;
		MakeTextureMap(MyMap);
		AsyncTask(ENamedThreads::GameThread, [&, MyMap] ()
		{
			DynamicTexture = FDynamicTexture(FIntPoint(PlaneSize.X / PerlinResolution, PlaneSize.Y / PerlinResolution), FLinearColor::White);
			DynamicTexture.SetTextureFromData(MyMap);
			DisplayTexture(DynamicTexture.GetTexture());
		});
	});
}


void APerlinNoise::MakeTextureMap(TMap<FIntPoint, FLinearColor>& OutMap) const
{
	SCOPED_NAMED_EVENT(HeyJude, FColor::Green);
	SCOPE_CYCLE_COUNTER(STAT_GenerateTexture);
	
	const float TempRandomSeed = FMath::FRandRange(-1.f, 1.f);

	OutMap = TMap<FIntPoint, FLinearColor>();
	for (int i = 0; i < PlaneSize.X / PerlinResolution; i++)
	{
		for (int j = 0; j < PlaneSize.Y / PerlinResolution; j++)
		{
			FLinearColor MyColor;
			if (PerlinColorCurve)
			{
				MyColor = PerlinColorCurve->GetLinearColorValue(FmMath::PerlinNoise2D(TempRandomSeed, PerlinGridTileSize / PerlinResolution, FVector2D(i, j)));
			}
			else
				MyColor = FLinearColor::Black;
			OutMap.Emplace(FIntPoint(i,j), MyColor);
		}
	}
}