#ifndef _QUADTREE_POINTS_H_
#define _QUADTREE_POINTS_H_
#include "Quadtree.h"

class QuadtreePoints : public Quadtree
{
public:
	QuadtreePoints(SDL_Rect area, uint level = 4, uint max_levels = 1, uint maxElements = 4);
	~QuadtreePoints();

private:

};





#endif // !_QUADTREE_POINTS_H_

