#pragma once

#include "FmEllipse.generated.h"

USTRUCT()
struct FmEllipse
{
	GENERATED_BODY()

	FmEllipse();
	FmEllipse(FVector InCenter, float InWidth, float InHeight);

	FVector Center = FVector();
	float Width = 0.f;
	float Height = 0.f;

	FVector GenerateRandomPoint(int InSeed);
};