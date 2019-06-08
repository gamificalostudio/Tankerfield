#include <math.h>
#include "MathUtils.h"

//Returns the interpolated value from the current value to the target value with the given a factor
float lerp(float curr_value, float trg_value, float factor)
{
	if (factor > 1.f)
	{
		factor = 1.f;
	}

	return 	curr_value * (1.f - factor) + trg_value * factor;
}

fPoint lerp(fPoint curr_value, fPoint trg_value, float factor)
{
	if (factor > 1.f)
	{
		factor = 1.f;
	}
	return 	curr_value * (1.f - factor) + trg_value * factor;
}

inline double Det(double a, double b, double c, double d)
{
	return a * d - b * c;
}

bool LineLineIntersect(fPoint A , //Line 1 start
	fPoint B, //Line 1 end
	fPoint C, //Line 2 start
	fPoint D, //Line 2 end
	fPoint &result) //Output 
{
	double detL1 = Det(A.x, A.y, B.x, B.y);
	double detL2 = Det(C.x, C.y, D.x, D.y);
	double x1mx2 = A.x - B.x;
	double x3mx4 = C.x - D.x;
	double y1my2 = A.y - B.y;
	double y3my4 = C.y - D.y;

	double xnom = Det(detL1, x1mx2, detL2, x3mx4);
	double ynom = Det(detL1, y1my2, detL2, y3my4);
	double denom = Det(x1mx2, y1my2, x3mx4, y3my4);

	if (denom == 0.0)//Lines don't seem to cross
	{
		result = { 0.f, 0.f };
		return false;
	}

	result.x = xnom / denom;
	result.y = ynom / denom;

	if (!isfinite(result.x) || !isfinite(result.y)) //Probably a numerical issue
		return false;

	return true; //All OK
}


bool get_line_intersection(fPoint p0, fPoint p1, fPoint p2, fPoint p3, fPoint *result)
{
	float s1_x, s1_y, s2_x, s2_y;
	s1_x = p1.x - p0.x;     s1_y = p1.y - p0.y;
	s2_x = p3.x - p2.x;     s2_y = p3.y - p2.y;

	float s, t;
	s = (-s1_y * (p0.x - p2.x) + s1_x * (p0.y - p2.y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = (s2_x * (p0.y - p2.y) - s2_y * (p0.x - p2.x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		// Collision detected
		if (result != NULL)
		{
			result->x = p0.x + (t * s1_x);
			result->y = p0.y + (t * s1_y);
		}
		
		return true;
	}

	return false; // No collision
}


//Clamps the rotation between 0 and 360 degrees
float ClampRotation(float angle)
{
	if (angle > 360)
	{
		angle = fmod(angle, 360);
	}
	else if (angle < -360)
	{
		angle = fmod(angle, -360);
	}
	if (angle < 0)
	{
		angle += 360;
	}
	return angle;
}
//
//template <typename T>
//
//inline T lerp(T v0, T v1, T t) {
//	return fma(t, v1, fma(-t, v0, v0));
//}