#include "Math_Utils.h"

float lerp(float v0, float v1, float t) {

	return (1.f - t)*v0 + t * v1;
}