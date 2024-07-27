#pragma once
#include "CoreMinimal.h"

class APerlinNoise;

class FMyThread : public FRunnable
{
public:
	
	FMyThread(APerlinNoise* InPerlinNoise);

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Exit() override;
	virtual void Stop() override;

	FRunnableThread* Thread;
	bool bShutdown= false;

	APerlinNoise* PerlinNoise = nullptr;
};
