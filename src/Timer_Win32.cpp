#include "Timer.h"

static double timerFrequency = 1.0 / 1000.0;

Timer::Timer()
{
	/*
	LARGE_INTEGER qpc;
	QueryPerformanceFrequency(&qpc);
	ASSERT(qpc.QuadPart > 0);

	timerFrequency = 1.0 / (double)qpc.QuadPart;
	*/
	timeBeginPeriod(1);
}

Timer::~Timer()
{
	timeEndPeriod(1);
}

double Timer::getTicks() const
{
	return timerFrequency * timeGetTime();
	/*
	LARGE_INTEGER qpc;
	QueryPerformanceCounter(&qpc);
	return timerFrequency * qpc.QuadPart;
	*/
}
