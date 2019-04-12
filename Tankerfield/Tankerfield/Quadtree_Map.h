#ifndef _QUADTREE_MAP_H_
#define _QUADTREE_MAP_H_

#include "Quadtree.h"
struct Tile
{
	Tile() {};
	Tile(uint id, SDL_Rect rect) :id(id), rect(rect)
	{}
		uint id;
		SDL_Rect rect;
		
};

class Quadtree_Map : public Quadtree
{

public:
	Quadtree_Map(SDL_Rect area, uint level = 0, uint max_levels = 1, uint maxElements = 4);

	void Draw() const override;

	//Return 0 if not added
	//1 Added 
	// 2 added on child
	// -1 error
	int InsertTile(Tile rect);

	void DrawMap(const Camera & camera);
private:
	std::list<Tile> elements;
	Quadtree_Map* nodes[4] = { nullptr,nullptr,nullptr,nullptr };

	void Subdivide() override;

};



#endif // !_QUADTREE_MAP_H_
