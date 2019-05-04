#ifndef _QUADTREE_RECT_H_
#define _QUADTREE_RECT_H_

class Quadtree;
class vector;
template<class TYPE>
struct Element
{
public:
	Element(SDL_Rect rect, TYPE data) : rect(rect), data(data)
	{};

	SDL_Rect rect;
	TYPE data;
};

template <class TYPE>
class Quadtree_rect :public Quadtree
{
public:
	Quadtree_rect(SDL_Rect area, uint level, uint max_levels);
	~Quadtree_rect();

	int ReturnNumbreOfLevels(int area_with, int camera_with, uint & levels);
	void InsertTile(SDL_Rect, TYPE);
	std::vector<TYPE> GetElementsIntersection(SDL_Rect camera);

private:
	std::vector<Element<TYPE>> elements;
	Quadtree_rect* childs[4]{ nullptr, nullptr, nullptr, nullptr };
	void Subdivide();




};
#endif // !_QUADTREE_RECT_H_



