#ifndef TIMER_H
#define TIMER_H

#include "common.h"

class Timer
{
public:
	Timer();
	~Timer();

	double getTicks() const;
};

#endif /* TIMER_H */
