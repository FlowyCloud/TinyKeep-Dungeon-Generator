
#include "LastDropProcgen/Struct/FmRandomStream.h"

#include <chrono>
#include <random>

#include "LastDropProcgen/Struct/FmMath.h"

FmRandomStream::FmRandomStream()
{
	Seed = 0;
	SdCoef = FmMath::GetSdCoef(ESdType::Percent_95);
	DistributionType = EDistributionType::Uniform;
}

FmRandomStream::FmRandomStream(ESeedType InSeedType, EDistributionType InDistributionType, ESdType InDeviationType)
{
	if (InSeedType == ESeedType::UseTime)
	{
		Seed = std::chrono::system_clock::now().time_since_epoch().count();
	}
	SdCoef = FmMath::GetSdCoef(InDeviationType);
	DistributionType = InDistributionType;
}
FmRandomStream::FmRandomStream(int32 InSeed, EDistributionType InDistributionType, ESdType InDeviationType)
{
	Seed = InSeed;
	SdCoef = FmMath::GetSdCoef(InDeviationType);
	DistributionType = InDistributionType;
}

FmRandomStream::FmRandomStream(FName InSeed, EDistributionType InDistributionType, ESdType InDeviationType)
{
	Seed = GetTypeHash(InSeed);
	SdCoef = FmMath::GetSdCoef(InDeviationType);
	DistributionType = InDistributionType;
}


// RandRange integer only work with uniform distribution
int FmRandomStream::RandRange(int Min, int Max)
{
	MutateSeed();
	std::mt19937 Generator(Seed);
	int OutRandomNumber = (Min + Max) / 2;

	switch (DistributionType)
	{
		case Uniform:
			std::uniform_int_distribution<int> Distribution{ Min, Max };
			OutRandomNumber = Distribution(Generator);
			break;
	}
	return OutRandomNumber;
}

float FmRandomStream::RandRange(float Min, float Max)
{
	MutateSeed();
	std::mt19937 Generator(Seed);
	const float Mean = (Max + Min) / 2;
	const float StandardDeviation = (Max - Min) * SdCoef;
	std::uniform_real_distribution<float> UniformDistribution{ Min, Max };
	std::normal_distribution<float> NormalDistribution{ Mean, StandardDeviation };

	switch (DistributionType)
	{
		case Uniform:
			return UniformDistribution(Generator);
		case Normal:
			return FMath::Clamp(NormalDistribution(Generator), Min, Max);
	}
	return 0.f;
}

double FmRandomStream::RandRange(double Min, double Max)
{
	MutateSeed();
	std::mt19937 Generator(Seed);
	double Mean = (Max + Min) / 2;
	double StandardDeviation = (Max - Min) * SdCoef;
	std::uniform_real_distribution<double> UniformDistribution{ Min, Max };
	std::normal_distribution<double> NormalDistribution{ Mean, StandardDeviation };

	switch (DistributionType)
	{
		case Uniform:
			return UniformDistribution(Generator);
		case Normal:
			return FMath::Clamp(NormalDistribution(Generator), Min, Max);
	}
	return 0.0;
}

double FmRandomStream::RandMean(double InMean, double InStandardDeviation)
{
	MutateSeed();
	std::mt19937 Generator(Seed);
	std::normal_distribution<double> Distribution{ InMean, InStandardDeviation };
	return Distribution(Generator);
}

void FmRandomStream::MutateSeed()
{
	FmMath::MutateSeed(Seed);
}

int FmRandomStream::GetSeed()
{
	FmMath::MutateSeed(Seed);
	return Seed;
}

