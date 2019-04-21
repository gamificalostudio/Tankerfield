#include "Quadtree_Points.h"

QuadtreePoints::QuadtreePoints(SDL_Rect area, uint level , uint max_levels , uint maxElements) :
	Quadtree( area,  level,  max_levels ,  maxElements )
{
}

QuadtreePoints::~QuadtreePoints()
{
}
