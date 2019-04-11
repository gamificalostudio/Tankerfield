

#include "Quadtree.h"
#include "M_Render.h"
#include "App.h"

template <class TYPE>
Quadtree<TYPE>::Quadtree(uint max_levels, SDL_Rect area, uint level, uint maxElements)
	:max_levels(max_levels), level(level), area(area), maxElements(maxElements)
{
	isDivided = false;
}

template <class TYPE>
Quadtree<TYPE>::~Quadtree()
{
}

template <class TYPE>
bool Quadtree<TYPE>::CheckIn(SDL_Rect rect)
{

	return(rect.x > area.x && rect.y > area.y && rect.x + rect.w < area.x + area.w 
		&& rect.y + rect.h < area.y + area.h);
}


