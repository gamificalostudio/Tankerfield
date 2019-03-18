#ifndef __J1TIMER_H__
#define __J1TIMER_H__

#include "p2Defs.h"

class j1Timer
{
public:

	// Constructor
	j1Timer();

	void Start();
	uint32 Read() const;
	float ReadSec() const;

private:
	uint32	started_at;
};

#endif /* __J1TIMER_H__ */