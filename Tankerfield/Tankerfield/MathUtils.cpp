#include "MathUtils.h"

//Returns the interpolated value from the current value to the target value with the given a factor
float lerp(float curr_value, float trg_value, float factor) {

	return (1.f - factor)*curr_value + factor * trg_value;
}