#ifndef _QUADTREE_MAP_H_
#define _QUADTREE_MAP_H_

#include "Quadtree.h"
#include "M_Textures.h"
struct Tile
{
	Tile() {};
	Tile(uint id, SDL_Rect rect) :id(id), rect(rect)
	{}
		uint id;
		SDL_Rect rect;
		SDL_Rect section;
		uint layer;
		uint sorting_value;
		SDL_Texture* texture = nullptr;
};

class Quadtree_Map : public Quadtree
{

public:
	Quadtree_Map(SDL_Rect area, uint level = 0, uint max_levels = 1, uint maxElements = 4);
	~Quadtree_Map();
	void Draw() const override;

	//Return 0 if not added
	//1 Added 
	// 2 added on child
	// -1 error
	int InsertTile(Tile rect);

	 std::vector<Tile> GetTilesIntersection(const Camera & camera);

	int compare(const Tile& a, const Tile& b)
	{
		if (a.sorting_value < b.sorting_value) return -1;
		else if (a.sorting_value == b.sorting_value) return 0;
		else return 1;
	}

	int ReturnNumbreOfLevels(int area_with, int camera_with, uint& levels);

private:
	std::vector<Tile> elements;
	Quadtree_Map* nodes[4] = { nullptr,nullptr,nullptr,nullptr };

	void Subdivide() override;
};



#endif // !_QUADTREE_MAP_H_

