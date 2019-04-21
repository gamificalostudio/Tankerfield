// ----------------------------------------------------
// Point class    -----------
// ----------------------------------------------------

#ifndef __POINT_H__
#define __POINT_H__

#include <math.h>

#include "Defs.h"

template<class TYPE1>
class Point
{
public:

	TYPE1 x, y;

	Point()
	{}

	Point(const Point<TYPE1>& v):
		x(v.x),
		y(v.y)
	{}

	Point(const TYPE1& x, const TYPE1& y):
		x(x),
		y(y)
	{}

	Point& create(const TYPE1& x, const TYPE1& y)
	{
		this->x = x;
		this->y = y;

		return(*this);
	}

	// Math ------------------------------------------------
	const Point& operator = (const Point &v) {
		x = v.x;
		y = v.y;

		return(*this);
	}
	
	Point operator +(const Point &v) const
	{
		Point r;

		r.x = x + v.x;
		r.y = y + v.y;

		return(r);
	}

	const Point& operator +=(const Point &v)
	{
		x += v.x;
		y += v.y;

		return(*this);
	}

	Point operator -(const Point &v) const
	{
		Point r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	Point operator -() const
	{
		Point r;

		r.x = -x;
		r.y = -y;

		return(r);
	}

	const Point& operator -=(const Point &v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	Point operator* (const TYPE1 & num) const {
		Point r;
		r.x = x * num;
		r.y = y * num;
		return r;
	}

	Point operator* (const Point & v) const {
		Point r;
		r.x = x * v.x;
		r.y = y * v.y;
		return r;
	}

	const Point operator*= (const TYPE1 & num) {

		x *= num;
		y *= num;
		return (*this);
	}

	Point operator/ (const TYPE1 & num) const {
		Point r;
		r.x = x / num;
		r.y = y / num;
		return r;
	}

	const Point operator /=(const TYPE1& v)
	{
		x /= v;
		y /= v;
		return (*this);
	}

	bool operator ==(const Point& v) const
	{
		return (x == v.x && y == v.y);
	}

	bool operator !=(const Point& v) const
	{
		return (x != v.x || y != v.y);
	}

	// Utils ------------------------------------------------
	bool IsZero() const
	{
		return (x == 0 && y == 0);
	}

	Point& SetToZero()
	{
		x = y = 0;
		return(*this);
	}

	Point& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	void Normalize() {

		double module = sqrt(x * x + y * y);
		if (module != 0.)
		{
			x /= module;
			y /= module;
		}
	}

	//Rotate a vector in radians
	Point Rotate(float angle) {
		Point aux = (*this);
		float angle_cos = cosf(angle);
		float angle_sin = sinf(angle);
		x = aux.x * angle_cos - aux.y * angle_sin;
		y = aux.x * angle_sin + aux.y * angle_cos;
		return(*this);
	}

	//Rotate a vector in degrees
	Point RotateDegree(float angle) {
		angle *= DEGTORAD;
		Point aux = (*this);
		float angle_cos = cosf(angle);
		float angle_sin = sinf(angle);
		x = aux.x * angle_cos - aux.y * angle_sin;
		y = aux.x * angle_sin + aux.y * angle_cos;
		return(*this);
	}

	explicit operator Point<int> () const
	{
		return Point<int>((int)x, (int)y);
	}

	explicit operator Point<float> () const
	{
		return Point<float>((float)x, (float)y);
	}

	// Distances ---------------------------------------------
	TYPE1 DistanceTo(const Point& v) const
	{
		TYPE1 fx = x - v.x;
		TYPE1 fy = y - v.y;

		return sqrtf((fx*fx) + (fy*fy));
	}

	TYPE1 DistanceNoSqrt(const Point& v) const
	{
		TYPE1 fx = x - v.x;
		TYPE1 fy = y - v.y;

		return (fx*fx) + (fy*fy);
	}

	TYPE1 DistanceManhattan(const Point& v) const
	{
		return abs(v.x - x) + abs(v.y - y);
	}
};

typedef Point<int> iPoint;
typedef Point<float> fPoint;

#endif // __P2POINT_H__