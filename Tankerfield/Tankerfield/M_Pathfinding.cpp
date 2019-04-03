#include "Brofiler\Brofiler.h"
#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_Pathfinding.h"
#include "M_input.h"
#include "M_render.h"
#include "M_Textures.h"
#include "M_map.h"

M_Pathfinding::M_Pathfinding() : Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.assign("pathfinding");
}

// Destructor
M_Pathfinding::~M_Pathfinding()
{
	RELEASE_ARRAY(map);
}

bool M_Pathfinding::Start()
{
	path_tex = app->tex->Load("maps/path.png");

	return true;
}

bool M_Pathfinding::PostUpdate(float dt)
{
	if (test_path)
	{
		static iPoint origin;
		static bool origin_selected = false;
		static bool createdDebugPath = false;

		iPoint mousePos;
		app->input->GetMousePosition(mousePos.x, mousePos.y);
		iPoint p = app->render->ScreenToWorld(mousePos.x, mousePos.y);
		p = app->map->MapToScreenI(p.x , p.y );
		
		if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN)
		{
			if (origin_selected == true)
			{

				origin_selected = false;

				if (CreatePath(origin, p) != -1)
				{
					createdDebugPath = true;
				}

			}
			else
			{
				origin = p;
				origin_selected = true;
				createdDebugPath = false;
				debugPath.clear();

			}
		}

		if (createdDebugPath)
		{
			uint debugPathSize = debugPath.size();
			if (debugPathSize == 0)
			{

				const std::vector<iPoint>* path = app->pathfinding->GetLastPath();
				uint sizeArray = path->size();
				for (uint i = 0; i < sizeArray; ++i)
				{
					debugPath.push_back(path->at(i));
				}
			}
			else
			{
				for (uint i = 0; i < debugPathSize; ++i)
				{
					iPoint pos = app->map->MapToScreenI(debugPath.at(i).x, debugPath.at(i).y);
					app->render->Blit(path_tex, pos.x, pos.y);
				}
			}

		}

		p = app->map->MapToScreenI(p.x, p.y);

		app->render->Blit(path_tex, p.x, p.y + 16);
	}
	

	return true;
}

// Called before quitting
bool M_Pathfinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.clear();

	RELEASE_ARRAY(map);

	return true;
}

// Sets up the walkability map
void M_Pathfinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool M_Pathfinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool M_Pathfinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar M_Pathfinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const std::vector<iPoint>* M_Pathfinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
const PathNode* PathList::Find(const iPoint& point) const
{
	std::list<PathNode>::const_iterator item = list.begin();
	while (item != list.end())
	{
		if (item->pos == point)
			return &(*item);

		item++;
	}

	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
const PathNode* PathList::GetNodeLowestScore() const
{
	const PathNode* ret = NULL;
	int min = 65535;

	std::list<PathNode>::const_reverse_iterator item = list.rbegin();
	while (item != list.rend())
	{
		if (item->Score() < min)
		{
			min = item->Score();
			ret = &(*item);
		}

		item++;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.size();

	// north
	cell.create(pos.x, pos.y + 1);
	if (app->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (app->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (app->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (app->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int M_Pathfinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	
	BROFILER_CATEGORY("CRATE PATH", Profiler::Color::LightBlue);
	last_path.clear();

	int ret = -1;

	if (!IsWalkable(origin) || !IsWalkable(destination))
	{
		ret = -1;
	}
	else
	{
		PathList open;
		PathList close;

		PathNode origin_tile(0, origin.DistanceNoSqrt(destination), origin, nullptr);
		open.list.push_back(origin_tile);

		while (open.list.size() > 0)
		{
			PathNode* current_node = (PathNode*)open.GetNodeLowestScore();
			close.list.push_back(*current_node);

			// It is needed to create a temporary list in order to use the erase() function in order to find it.
			for (std::list<PathNode>::iterator item = open.list.begin(); item != open.list.end(); item++)
			{
				if (&(*item) == &(*current_node))
				{
					open.list.erase(item);
					break;
				}
			}

			// Important: back() returns a reference of the last element, and end() returns an iterator to the last element. Therefore, we use back().
			if (close.list.back().pos == destination)
			{
				for (PathNode it = close.list.back(); it.parent != nullptr; it = *close.Find(it.parent->pos))
				{
					last_path.push_back(it.pos);

					if (it.parent == nullptr)
					{
						last_path.push_back(close.list.front().pos);
					}
				}

				std::reverse(last_path.begin(), last_path.end());

				return last_path.size();
			}
			else
			{
				PathList neighbours;
				close.list.back().FindWalkableAdjacents(neighbours);

				std::list<PathNode>::iterator it = neighbours.list.begin();

				while (it != neighbours.list.end())
				{
					if (close.Find(it->pos) != NULL)
					{
						it++;
						continue;
					}
					else if (open.Find(it->pos) != NULL)
					{
						PathNode temp_path = *open.Find(it->pos);

						it->CalculateF(destination);
						if (temp_path.g > it->g)
						{
							temp_path.parent = it->parent;
						}
					}
					else
					{
						it->CalculateF(destination);
						open.list.push_back(*it);
					}

					it++;
				}

				neighbours.list.clear();
			}
		}
	}

	return ret;
}
