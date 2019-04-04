// ----------------------------------------------------
// Point class    -----------
// ----------------------------------------------------

#ifndef __POINT_H__
#define __POINT_H__

#include "Defs.h"
#include <math.h>

template<class TYPE>
class Point
{
public:

	TYPE x, y;

	Point()
	{}

	Point(const Point<TYPE>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	Point(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;
	}

	Point& create(const TYPE& x, const TYPE& y)
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

	const Point& operator -=(const Point &v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	Point operator* (const TYPE & num) const {
		Point r;
		r.x = x * num;
		r.y = y * num;
		return r;
	}

	const Point operator*= (const TYPE & num) {
		x *= num;
		y *= num;
		return (*this);
	}

	Point operator/ (const TYPE & num) const {
		Point r;
		r.x = x / num;
		r.y = y / num;
		return r;
	}

	const Point operator /=(const TYPE& v)
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
	void Rotate(float angle) {
		Point aux = (*this);
		float angle_cos = cosf(angle);
		float angle_sin = sinf(angle);
		x = aux.x * angle_cos - aux.y * angle_sin;
		y = aux.x * angle_sin + aux.y * angle_cos;
	}

	//Rotate a vector in degrees
	void RotateDegree(float angle) {
		angle *= DEGTORAD;
		Point aux = (*this);
		float angle_cos = cosf(angle);
		float angle_sin = sinf(angle);
		x = aux.x * angle_cos - aux.y * angle_sin;
		y = aux.x * angle_sin + aux.y * angle_cos;

	}
	
	//Linear interpolation

	float lerp(float posx, float posy, float t){

		return (1.f - t)*posx + t * posy;
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
	TYPE DistanceTo(const Point& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return sqrtf((fx*fx) + (fy*fy));
	}

	TYPE DistanceNoSqrt(const Point& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (fx*fx) + (fy*fy);
	}

	TYPE DistanceManhattan(const Point& v) const
	{
		return abs(v.x - x) + abs(v.y - y);
	}
};

typedef Point<int> iPoint;
typedef Point<float> fPoint;

#endif // __P2POINT_H__