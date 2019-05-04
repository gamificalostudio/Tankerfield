#include "Quadtree_Points.h"


QuadtreePoints::QuadtreePoints(SDL_Rect area, uint level , uint max_levels) :
	Quadtree( area,  level,  max_levels )
{
}

QuadtreePoints::~QuadtreePoints()
{
}
