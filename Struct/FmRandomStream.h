#pragma once
#include "LastDropProcgen/GlobalHeader/QyEnum.h"

#include "FmRandomStream.generated.h"

USTRUCT()
struct FmRandomStream
{
	GENERATED_BODY()

	FmRandomStream();
	FmRandomStream(ESeedType InSeedType, EDistributionType InDistributionType, ESdType InDeviationType = ESdType::Percent_95);
	FmRandomStream(int32 InSeed, EDistributionType InDistributionType, ESdType InDeviationType = ESdType::Percent_95);
	FmRandomStream(FName InSeed, EDistributionType InDistributionType, ESdType InDeviationType = ESdType::Percent_95);

private:
	int32 Seed;
	double SdCoef;
	EDistributionType DistributionType;
public:

	int RandRange(int Min, int Max);
	float RandRange(float Min, float Max);
	double RandRange(double Min, double Max);
	double RandMean(double InMean, double InStandardDeviation);
	void MutateSeed();
	
	int GetSeed();
};