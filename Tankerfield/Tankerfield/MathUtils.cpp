#include <math.h>

#include "MathUtils.h"

//Returns the interpolated value from the current value to the target value with the given a factor
float lerp(float curr_value, float trg_value, float factor) {

	return (1.f - factor)*curr_value + factor * trg_value;
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