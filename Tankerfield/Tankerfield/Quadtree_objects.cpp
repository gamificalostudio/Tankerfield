#ifndef _QUADTREE_RECT_H_
#define _QUADTREE_RECT_H_

#include "SDL/include/SDL_rect.h"
#include "Quadtree.h"
#include <vector>


struct Element
{
public:
	Element(SDL_Rect rect, void* data) : rect(rect), data(data)
	{};

	SDL_Rect rect;
	void* data;
};

template <class TYPE>
class Quadtree_rect :public Quadtree
{
private:
	std::vector<Element> elements;
	Quadtree_rect<TYPE>* childs[4]{ nullptr, nullptr, nullptr, nullptr };

	void Subdivide()
	{
		isDivided = true;

		int half_width = area.w*0.5f, half_height = area.h*0.5f;

		childs[(int)QUADRANT::NORTH_EAST] = DBG_NEW Quadtree_rect<TYPE>({ area.x + half_width , area.y ,half_width,half_height }, level + 1, max_levels);

		childs[(int)QUADRANT::NORTH_WEST] = DBG_NEW Quadtree_rect<TYPE>({ area.x, area.y, half_width, half_height }, level + 1, max_levels);

		childs[(int)QUADRANT::SOUTH_EAST] = DBG_NEW Quadtree_rect<TYPE>({ area.x + half_width, area.y + half_height, half_width, half_height }, level + 1, max_levels);

		childs[(int)QUADRANT::SOUTH_WEST] = DBG_NEW Quadtree_rect<TYPE>({ area.x, area.y + half_height, half_width, half_height }, level + 1, max_levels);
	}

	inline bool CheckIntersection(const SDL_Rect& rect, const SDL_Rect& rect2)
	{
		return !(rect.x >= (rect2.x + rect2.w) || (rect.x + rect.w) <= rect2.x || rect.y >= (rect2.y + rect2.h) || (rect.y + rect.h) <= rect2.y);
	}

public:

	Quadtree_rect(SDL_Rect area, uint level, uint max_levels) : Quadtree(area, level, max_levels)
	{};

	~Quadtree_rect()
	{
		for (uint i = 0; i < 4; ++i)
		{
			if (childs[i] != nullptr)
			{
				delete childs[i];
				childs[i] = nullptr;
			}
		}

		elements.clear();
	}

	int ReturnNumbreOfLevels(int area_with, int camera_with, uint & levels)
	{
		int new_area = area_with * 0.25;
		if (new_area <= camera_with)
			return levels;
		else
			ReturnNumbreOfLevels(new_area, camera_with, ++levels);
	}
	void InsertElement(const SDL_Rect& rect, const TYPE& data)
	{
		if (!SDL_HasIntersection(&rect, &area))
		{
			return;
		}
		else
		{
			if (level >= max_levels)
			{
				Element new_element(rect, data);
				elements.push_back(new_element);
			}
			else
			{
				if (!this->isDivided)
				{
					this->Subdivide();
				}

				for (uint i = 0; i < 4; ++i)
				{
					childs[i]->InsertElement(rect, data);
				}
			}
		}
	}
	std::vector<TYPE> GetElementsIntersection(SDL_Rect camera)
	{
		std::vector<TYPE> ret;

		if (CheckIntersection(camera, area))
		{
			if (!isDivided)
			{
				for (std::vector<Element>::iterator i = elements.begin(); i != elements.end(); ++i)
				{
					if (CheckIntersection(camera, (*i).rect))
					{
						ret.push_back((TYPE)(*i).data);
					}
				}
				return ret;
			}
			else
			{
				for (uint i = 0; i < 4; ++i)
				{
					std::vector<TYPE> aux = childs[i]->GetElementsIntersection(camera);
					if (aux.size() > 0)
						ret.insert(ret.end(), aux.begin(), aux.end());
				}
			}
		}
		return ret;
	}


};

#endif // !_QUADTREE_RECT_H_