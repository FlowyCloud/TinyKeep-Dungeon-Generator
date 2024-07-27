
#include "LastDropProcgen/Struct/FmEllipse.h"
#include "FmRandomStream.h"

FmEllipse::FmEllipse()
{
}

FmEllipse::FmEllipse(FVector InCenter, float InWidth, float InHeight)
{
	Center = InCenter;
	Width = InWidth;
	Height = InHeight;
}

FVector FmEllipse::GenerateRandomPoint(int InSeed)
{
	FmRandomStream RandomStream = FmRandomStream(InSeed, EDistributionType::Uniform);
	float Angle = 2 * PI * RandomStream.RandRange(0.f, 1.f);
	float u = RandomStream.RandRange(0.f, 1.f) + RandomStream.RandRange(0.f, 1.f);

	float r;
	if (u > 1)
		r = 2 - u;
	else
		r = u;

	float OutX = Center.X + r * Width * cos(Angle);
	float OutY = Center.Y + r * Height * sin(Angle);
	
	return FVector(OutX, OutY, Center.Z);
}