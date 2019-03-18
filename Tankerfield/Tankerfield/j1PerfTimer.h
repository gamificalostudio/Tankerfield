#ifndef __J1PERFTIMER_H__
#define __J1PERFTIMER_H__

#include "p2Defs.h"

class j1PerfTimer
{
public:

	// Constructor
	j1PerfTimer();

	void Start();
	double ReadMs() const;
	uint64 ReadTicks() const;

private:
	uint64	started_at;
	static uint64 frequency;
};

#endif /* __J1PERFTIMER_H__ */