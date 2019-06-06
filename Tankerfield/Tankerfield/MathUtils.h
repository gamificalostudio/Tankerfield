#ifndef __MATH_UTILS_H__
#define __MATH_UTILS_H__

#include "Point.h"

float lerp(float v0, float v1, float t);
fPoint lerp(fPoint v0, fPoint v1, float t);
float ClampRotation(float angle);

bool get_line_intersection(fPoint p0, fPoint p1, fPoint p2, fPoint p3, fPoint *result);

bool LineLineIntersect(fPoint A, //Line 1 start
	fPoint B, //Line 1 end
	fPoint C, //Line 2 start
	fPoint D, //Line 2 end
	fPoint &result); //Output 

#endif //__MATH_UTILS_H__
