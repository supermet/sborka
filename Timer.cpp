#include "Timer.h"
Timer::Timer(TIMERPROC pr)
{
	interval=1000;
	proc=pr;
	enabled=false;
}

void Timer::start()
{
	if (!enabled)
	{
		timerid=SetTimer(0, 0, interval, proc);
		enabled=true;
	}
}
void Timer::stop()
{
	if (enabled)
	{
		KillTimer(0, timerid);
		enabled=false;
	}
}


void Timer::setInterval(UINT interv)
{
    interval = interv;
}

UINT Timer::getInterval()
{
    return interval;
}

Timer::~Timer()
{
	stop();
	//dtor
}
