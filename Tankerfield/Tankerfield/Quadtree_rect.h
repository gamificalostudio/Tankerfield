#ifndef _QUADTREE_RECT_H_
#define _QUADTREE_RECT_H_

class Quadtree;
class vector;

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

	
public:
	Quadtree_rect(SDL_Rect area, uint level, uint max_levels);
	~Quadtree_rect();

	int ReturnNumbreOfLevels(int area_with, int camera_with, uint & levels);
	void InsertElement(const SDL_Rect&, const TYPE& data);
	std::vector<TYPE> GetElementsIntersection(SDL_Rect camera);

private:
	std::vector<Element> elements;
	Quadtree_rect* childs[4]{ nullptr, nullptr, nullptr, nullptr };
	void Subdivide();

	inline bool CheckIntersection(const SDL_Rect& rect, const SDL_Rect& rect2)
	{
		return !(rect.x >= (rect2.x + rect2.w) || (rect.x + rect.w) <= rect2.x || rect.y >= (rect2.y + rect2.h) || (rect.y + rect.h) <= rect2.y);
	}
};

#endif // !_QUADTREE_RECT_H_



