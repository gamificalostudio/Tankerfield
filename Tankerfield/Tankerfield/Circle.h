#ifndef _CICLE_H_
#define _CICLE_H_
#include "Point.h"
struct Circle
{
	fPoint center;
	float radius;
	bool IsPointIn(fPoint point)
	{
		return (point.DistanceNoSqrt(center) < radius * radius);
	}
};


#endif // !_CICLE_H_