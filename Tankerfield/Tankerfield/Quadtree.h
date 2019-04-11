#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include <list>
#include "Defs.h"
#include "Log.h"
#include "SDL/include/SDL.h"

enum class QUADRANT
{
	NORTH_EAST,
	NORTH_WEST,
	SOUTH_EAST,
	SOUTH_WEST,
};
template  <class TYPE>
class Quadtree 
{
private:

	uint max_levels;		//Max number of levels a quadtree will have

	uint level;				//Position in the tree

	SDL_Rect area;		//Position and size

	bool isDivided;			//true when have subnodes

	uint maxElements; //max elements to divide;

	std::list<TYPE> elements;

	Quadtree<TYPE>* nodes[4] = { nullptr,nullptr,nullptr,nullptr };

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
	virtual void Subdivide() 
	{
		isDivided = true;

		int half_width = area.w*0.5f, half_height= area.h*0.5f;

		nodes[(int)QUADRANT::NORTH_EAST] = new Quadtree({area.x + half_width , area.y ,half_width,half_height });

		nodes[(int)QUADRANT::NORTH_WEST] = new Quadtree({ area.x, area.y, half_width, half_height });

		nodes[(int)QUADRANT::SOUTH_EAST] = new Quadtree({ area.x + half_width, area.y + half_height, half_width, half_height });

		nodes[(int)QUADRANT::SOUTH_WEST] = new Quadtree({ area.x, area.y + half_height, half_width, half_height });
	};

	void Draw() const {};

	//Return 0 if not added
	//1 Added 
	// 2 added on child
	// -1 error
	int AddElement(TYPE element, SDL_Rect rect)
	{
		int ret = 0;
		if (!CheckIn(rect))
		{
			return ret;
		}
		else
		{
			
			if (elements.size < maxElements)
			{
				this->elements.push_back(element);
			}
			else if(!this->Subdivide)
			{
				this->Subdivide();
			}
			for (uint i = 0; i < 4; ++i)
			{
				if (nodes[i]->AddElement(element, rect) == 1)
				{
					return ret = 2;
				}
			}

		}
		return ret = -1;
	}
	bool CheckIn(SDL_Rect rect) {
		return(rect.x > area.x && rect.y > area.y && rect.x + rect.w < area.x + area.w
			&& rect.y + rect.h < area.y + area.h);
	};


};

#endif // !__QUADTREE_H__

