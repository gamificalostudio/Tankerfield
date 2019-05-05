#include "Quadtree_Map.h"
#include "M_Map.h"
Quadtree_Map::Quadtree_Map(SDL_Rect area, uint level, uint max_levels):
	Quadtree(area, level, max_levels)
{

}
Quadtree_Map::~Quadtree_Map()
{
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

void Quadtree_Map::InsertTile(Tile tile)
{
	if (!SDL_HasIntersection(&tile.rect, &area))
	{
		return;
	}
	else
	{
		//if this node is the lowest level insert here
		if (level >= max_levels)
		{
			elements.push_back(tile);
			return;
		}
		else
		{

			if (!this->isDivided)
			{
				this->Subdivide();
			}
			for (uint i = 0; i < 4; ++i)
			{
				nodes[i]->InsertTile(tile);
			}
		}


	}
}

std::vector<Tile> Quadtree_Map::GetTilesIntersection(const Camera &camera)
{
	std::vector<Tile> ret;

	if (CheckIntersection(camera.rect, area))
	{
		if (!isDivided)
		{
			
			for (std::vector<Tile>::iterator i = elements.begin(); i != elements.end(); ++i)
			{
				if (CheckIntersection(camera.rect, (*i).rect))
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
