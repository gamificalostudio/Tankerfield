#include "Quadtree_Map.h"
#include "M_Map.h"
Quadtree_Map::Quadtree_Map(SDL_Rect area, uint level, uint max_levels, uint maxElements) :
	Quadtree(area, level, max_levels, maxElements)
{

};

void Quadtree_Map::Subdivide()
{
	isDivided = true;

	int half_width = area.w*0.5f, half_height = area.h*0.5f;

	nodes[(int)QUADRANT::NORTH_EAST] = new Quadtree_Map({ area.x + half_width , area.y ,half_width,half_height });

	nodes[(int)QUADRANT::NORTH_WEST] = new Quadtree_Map({ area.x, area.y, half_width, half_height });

	nodes[(int)QUADRANT::SOUTH_EAST] = new Quadtree_Map({ area.x + half_width, area.y + half_height, half_width, half_height });

	nodes[(int)QUADRANT::SOUTH_WEST] = new Quadtree_Map({ area.x, area.y + half_height, half_width, half_height });
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
	if (SDL_HasIntersection(&tile.rect,&area))
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

void Quadtree_Map::DrawMap(SDL_Rect camera)
{
	if (SDL_HasIntersection(&camera, &area))
	{
		if (!isDivided)
		{
			for (std::list<Tile>::iterator i = elements.begin(); i != elements.end(); ++i)
			{
				Tile tile = (*i);
				TileSet* tileset = app->map->GetTilesetFromTileId(tile.id);
				SDL_Rect rect = tileset->GetTileRect(tile.id);

				app->render->Blit(tileset->texture, tile.rect.x, tile.rect.y, &rect);
			}
		}
		else
		{
			for (uint i = 0; i < 4; ++i)
			{
				nodes[i]->DrawMap(camera);
			}
		}
	}
};
