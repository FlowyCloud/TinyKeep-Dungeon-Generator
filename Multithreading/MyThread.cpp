#include "LastDropProcgen/Multithreading/MyThread.h"

#include "LastDropProcgen/Actor/Noise/PerlinNoise.h"

FMyThread::FMyThread(APerlinNoise* InPerlinNoise)
{
	PerlinNoise = InPerlinNoise;
	Thread = FRunnableThread::Create(this, TEXT("MyThread"));
}

bool FMyThread::Init() {
	/* Should the thread start? */
	return true;
}

uint32 FMyThread::Run() {
	while (!bShutdown) {
		/* Work on a dedicated thread */
	}
	return 0;
}

void FMyThread::Exit() {
	/* Post-Run code, threaded */
}

void FMyThread::Stop() {
	bShutdown = true;
}
