#pragma once

#define PRINT(Color, Text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Color, TEXT(Text));
#define PRINTF(Duration, Text, ...)\
if (GEngine) GEngine->AddOnScreenDebugMessage(-1, Duration, FColor::Red, FString::Printf(TEXT(Text), ##__VA_ARGS__));

DECLARE_STATS_GROUP(TEXT("ProcGen"), STATGROUP_ProcGen, STATCAT_MyGroup);