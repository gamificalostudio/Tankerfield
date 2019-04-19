#ifndef __RECT_H__
#define __RECT_H__

#include "Point.h"
#include "SDL/include/SDL_rect.h"

template<class TYPE1, class TYPE2> 
class Rect
{
public:
	Point<TYPE1> pos;
	TYPE2 w, h;

	Rect()
	{}

	Rect(const Rect<TYPE1, TYPE2>& r) :
		w(r.w),
		h(r.h)
	{
		pos.x = r.pos.x;
		pos.y = r.pos.y;
	}

	Rect(const TYPE1& x, const TYPE1& y, const TYPE2 & w, const TYPE2 & h) :
		w(w),
		h(h)
	{
		pos.x = x;
		pos.y = y;
	}

	Rect& create(const TYPE1& x, const TYPE1& y, const TYPE2 & w, const TYPE2 & h)
	{
		this->pos.x = x;
		this->pos.y = y;
		this->w = w;
		this->h = h;

		return(*this);
	}

	const Rect& operator = (const Rect &r)
	{
		pos.x = r.pos.x;
		pos.y = r.pos.y;
		w = r.w;
		h = r.h;

		return(*this);
	}

	bool operator ==(const Rect& r) const
	{
		return (pos.x == r.pos.x && pos.y == r.pos.y && w == r.w && h == r.h);
	}

	bool operator !=(const Rect& r) const
	{
		return (pos.x != r.pos.x || pos.y != r.pos.y || w != r.w || h != r.h);
	}

	TYPE1 GetTop() const
	{
		return pos.y;
	}

	TYPE1 GetBottom() const
	{
		return pos.y + h;
	}

	TYPE1 GetLeft() const
	{
		return pos.x;
	}

	TYPE1 GetRight() const
	{
		return pos.x + w;
	}

	explicit operator Rect<int, int>() const
	{
		return Rect<int, int>((int)pos.x, (int)pos.y, (int)w, (int)h);
	}

	explicit operator Rect<float, float>() const
	{
		return Rect<float, float>((float)pos.x, (float)pos.y, (float)w, (float)h);
	}

	explicit operator SDL_Rect () const
	{
		SDL_Rect r;
		r.x = pos.x;
		r.y = pos.y;
		r.w = w;
		r.h = h;
		return r;
	}
};

typedef Rect<int, int> iRect;
typedef Rect<float, float> fRect;

#endif