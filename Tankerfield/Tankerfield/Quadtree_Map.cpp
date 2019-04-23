#include "Quadtree_Map.h"
#include "M_Map.h"
Quadtree_Map::Quadtree_Map(SDL_Rect area, uint level, uint max_levels, uint maxElements) :
	Quadtree(area, level, max_levels, maxElements)
{

}
Quadtree_Map::~Quadtree_Map()
{
	this;
	for (uint i = 0; i < 4; ++i)
	{
		if (nodes[i] != nullptr)
		{
			delete nodes[i];
			nodes[i] = nullptr;
		}
	}

	elements.clear();


};

int Quadtree_Map::ReturnNumbreOfLevels(int area_with, int camera_with, uint & levels)
{
	int new_area = area_with * 0.25;
	if (new_area <= camera_with)
		return levels;
	else
		ReturnNumbreOfLevels(new_area, camera_with, ++levels);
}

void Quadtree_Map::Subdivide()
{
	isDivided = true;

	int half_width = area.w*0.5f, half_height = area.h*0.5f;

	nodes[(int)QUADRANT::NORTH_EAST] = DBG_NEW Quadtree_Map({ area.x + half_width , area.y ,half_width,half_height }, level+1, max_levels);

	nodes[(int)QUADRANT::NORTH_WEST] = DBG_NEW Quadtree_Map({ area.x, area.y, half_width, half_height }, level + 1, max_levels);

	nodes[(int)QUADRANT::SOUTH_EAST] = DBG_NEW Quadtree_Map({ area.x + half_width, area.y + half_height, half_width, half_height },	level + 1, max_levels);

	nodes[(int)QUADRANT::SOUTH_WEST] = DBG_NEW Quadtree_Map({ area.x, area.y + half_height, half_width, half_height }, level + 1, max_levels);
}
void Quadtree_Map::Draw() const
{
	app->render->DrawQuad(area, 0, 0, 255, 255, false);
	if (isDivided)
	{
		for (uint i = 0; i < 4; ++i)
		{
			nodes[i]->Draw();
		}
	}
}

int Quadtree_Map::InsertTile(Tile tile)
{
	int ret = 0;
	if (!SDL_HasIntersection(&tile.rect,&area))
	{
		return ret;
	}
	else
	{
		//if this node is the lowest level insert here
		if (level >= max_levels)
		{
			elements.push_back(tile);
			return 1;
		}
		else
		{
			
			if (!this->isDivided)
			{
				this->Subdivide();
			}
			for (uint i = 0; i < 4; ++i)
			{
				if (nodes[i]->InsertTile(tile) == 1)
				{
					return ret = 2;
				}
			}
		}
		

	}
	return ret = -1;
}

std::vector<Tile> Quadtree_Map::GetTilesIntersection(const Camera &camera)
{
	SDL_Rect cam = camera.rect;
	cam.y -= 75;
	cam.w += 60;
	cam.h += 75;
	std::vector<Tile> ret;

	if (SDL_HasIntersection(&cam, &area))
	{
		if (!isDivided)
		{
			
			for (std::vector<Tile>::iterator i = elements.begin(); i != elements.end(); ++i)
			{
				if (SDL_HasIntersection(&cam,&(*i).rect))
				{
					ret.push_back((*i));
				}
			}
			return ret;
		}
		else
		{
			for (uint i = 0; i < 4; ++i)
			{
				std::vector<Tile> aux = nodes[i]->GetTilesIntersection(camera);
				if(aux.size() > 0)
					ret.insert(ret.end(), aux.begin(), aux.end());
			}
		}
	}
	return ret;
};
