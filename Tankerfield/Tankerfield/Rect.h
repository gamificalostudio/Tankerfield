#ifndef __RECT_H__
#define __RECT_H__

#include "Point.h"

template<class TYPE>
class Rect
{
public:
	Point<TYPE> pos;
	TYPE w, h;

	Rect()
	{}

	Rect(const Rect<TYPE>& r) :
		x(r.pos.x),
		y(r.pos.y),
		w(r.w),
		h(r.h)
	{}

	Rect(const TYPE& x, const TYPE& y, const TYPE & w, const TYPE & h) :
		x(x),
		y(y),
		w(w),
		h(h)
	{}

	Rect& create(const TYPE& x, const TYPE& y, const TYPE & w, const TYPE & h)
	{
		this->pos.x = x;
		this->y = y;
		this->w = w;
		this->h = h;

		return(*this);
	}

	// Math ------------------------------------------------
	const Rect& operator = (const Rect &r) {
		x = r.x;
		y = r.y;
		w = r.w;
		h = r.h;

		return(*this);
	}

	bool operator ==(const Rect& r) const
	{
		return (x == r.x && y == r.y && w == r.w && h == r.h);
	}

	bool operator !=(const Rect& r) const
	{
		return (x != r.x || y != r.y || w != r.w || h != r.h);
	}

	// Utils ------------------------------------------------

	void Normalize() {
		double module = sqrt(x * x + y * y);
		if (module != 0.)
		{
			x /= module;
			y /= module;
		}
	}

	//Rotate a vector in radians
	void Rotate(float angle) {
		Rect aux = (*this);
		float angle_cos = cosf(angle);
		float angle_sin = sinf(angle);
		x = aux.x * angle_cos - aux.y * angle_sin;
		y = aux.x * angle_sin + aux.y * angle_cos;
	}

	//Rotate a vector in degrees
	void RotateDegree(float angle) {
		angle *= DEGTORAD;
		Rect aux = (*this);
		float angle_cos = cosf(angle);
		float angle_sin = sinf(angle);
		x = aux.x * angle_cos - aux.y * angle_sin;
		y = aux.x * angle_sin + aux.y * angle_cos;
	}

	explicit operator Rect<int>() const
	{
		return Rect<int>((int)x, (int)y);
	}

	explicit operator Rect<float>() const
	{
		return Rect<float>((float)x, (float)y);
	}

	// Distances ---------------------------------------------
	TYPE DistanceTo(const Rect& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return sqrtf((fx*fx) + (fy*fy));
	}

	TYPE DistanceNoSqrt(const Rect& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (fx*fx) + (fy*fy);
	}

	TYPE DistanceManhattan(const Rect& v) const
	{
		return abs(v.x - x) + abs(v.y - y);
	}
};

typedef Rect<int> iRect;
typedef Rect<float> fRect;

#endif