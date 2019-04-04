#ifndef _CICLE_H_
#define _CICLE_H_
#include "Point.h"
struct Circle
{
	fPoint center;
	float radius;
	bool IsPointIn(fPoint point)
	{
		if (point.x >= (center.x - radius)
			&& point.x <= (center.x + radius)
			&& point.y >= (center.y - radius)
			&& point.y <= (center.y + radius))
			return true;
		else
			return false;
	}
};


#endif // !_CICLE_H_