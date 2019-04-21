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