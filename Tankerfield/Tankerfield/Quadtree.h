#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include "Defs.h"
#include "Log.h"
#include "SDL/include/SDL.h"

template  <class TYPE>
class Quadtree 
{
protected:

	uint max_levels;		//Max number of levels a quadtree will have
	uint level;				//Position in the tree
	SDL_Rect area;		//Position and size
	bool isDivided;			//true when have subnodes
	uint maxElements; //max elements to divide;

public:
	//Constructor
	Quadtree( SDL_Rect area, uint level = 4, uint max_levels = 1, uint maxElements = 4) :max_levels(max_levels), 
		level(level),
		area(area),
		maxElements(maxElements),
		isDivided(false)
	{
	}

	~Quadtree() {};
	//subdivide the node into 4 subnodes
	virtual void Subdivide() {};

	void Draw() const {};

	bool CheckIn(SDL_Rect rect) {
		return(rect.x > area.x && rect.y > area.y && rect.x + rect.w < area.x + area.w
			&& rect.y + rect.h < area.y + area.h);
	};

};

#endif // !__QUADTREE_H__

