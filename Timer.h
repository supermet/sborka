#ifndef TIMER_H
#define TIMER_H
#include "windows.h"

class Timer
{
public:
	Timer(TIMERPROC pr);
	void start();
	void stop();
	void setInterval(UINT);
	UINT getInterval();
	virtual ~Timer();
protected:
private:
    UINT interval;
    bool enabled;
	UINT timerid;
	TIMERPROC proc;
};

#endif // TIMER_H
