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


	return true;
}

bool M_Pathfinding::PostUpdate(float dt)
{


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

bool M_Pathfinding::Reset()
{
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
	map = data;
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
	bool directions_state[8] = {false, false, false, false, false, false, false, false};
	uint before = list_to_fill.list.size();

	// NORTH
	cell.create(pos.x, pos.y + 1);
	if (cell.y < app->map->data.rows)
			if(directions_state[(int)DIRECTIONS::NORTH] = app->pathfinding->IsWalkable(cell))
				list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// EAST
	cell.create(pos.x + 1, pos.y);
	if(cell.x < app->map->data.columns)
		if (directions_state[(int)DIRECTIONS::EAST] = app->pathfinding->IsWalkable(cell))
			list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// SOUTH
	cell.create(pos.x, pos.y - 1);
	if(cell.y >= 0)
		if (directions_state[(int)DIRECTIONS::SOUTH] = app->pathfinding->IsWalkable(cell))
			list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// WEST
	cell.create(pos.x - 1, pos.y);
	if(cell.x >= 0)
		if (directions_state[(int)DIRECTIONS::WEST] = app->pathfinding->IsWalkable(cell))
			list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// NORTH_EAST
	if (directions_state[(int)DIRECTIONS::NORTH] && directions_state[(int)DIRECTIONS::EAST])
	{
		cell.create(pos.x + 1, pos.y + 1);
		if (directions_state[(int)DIRECTIONS::NORTH_EAST] = app->pathfinding->IsWalkable(cell))
			list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	}

	// SOUTH_EAST
	if (directions_state[(int)DIRECTIONS::SOUTH] && directions_state[(int)DIRECTIONS::EAST])
	{
		cell.create(pos.x + 1, pos.y - 1);
		if (directions_state[(int)DIRECTIONS::SOUTH_EAST] = app->pathfinding->IsWalkable(cell))
			list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	}

	// WEST_SOUTH
	if (directions_state[(int)DIRECTIONS::WEST] && directions_state[(int)DIRECTIONS::SOUTH])
	{
		cell.create(pos.x - 1, pos.y - 1);
		if (directions_state[(int)DIRECTIONS::WEST_SOUTH] = app->pathfinding->IsWalkable(cell))
			list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	}

	// WEST_NORTH
	if (directions_state[(int)DIRECTIONS::WEST] && directions_state[(int)DIRECTIONS::NORTH])
	{
		cell.create(pos.x - 1, pos.y + 1);
		if (directions_state[(int)DIRECTIONS::NORTH] = app->pathfinding->IsWalkable(cell))
			list_to_fill.list.push_back(PathNode(-1, -1, cell, this));
	}


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
	h = pos.DistanceNoSqrt(destination);

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

	if (origin == destination || !IsWalkable(origin) || !IsWalkable(destination) )
	{
		return ret = -1;
	}
	
		PathList open;
		PathList close;

		PathNode origin_tile(0, origin.DistanceNoSqrt(destination), origin, nullptr);
		open.list.push_back(origin_tile);

		while (open.list.size() > 0)
		{
			PathNode* current_node = (PathNode*)open.GetNodeLowestScore();
			close.list.push_back(*current_node);

			// Important: back() returns a reference of the last element, and end() returns an iterator to the last element. Therefore, we use back().
			if (current_node->pos == destination)
			{
				const PathNode* node = nullptr;
				for (node = &(*current_node); node->pos != origin; node = node->parent) {
					last_path.push_back(node->pos);
				}
				last_path.push_back(node->pos);
				std::reverse(last_path.begin(), last_path.end());
				return ret = last_path.size();
			}

			std::list<PathNode>::iterator it = open.list.begin();
			while (it != open.list.end()) {

				if (&(*it) == &(*current_node))
					break;
				++it;
			}
			open.list.erase(it);

			// TODO 5: Fill a list of all adjancent nodes
			PathList childs;
			close.list.back().FindWalkableAdjacents(childs);

			// TODO 6: Iterate adjancent nodes:					
			std::list<PathNode>::iterator child_node = childs.list.begin();
			while (child_node != childs.list.end())
			{
				if (!close.Find((*child_node).pos) != NULL)	// ignore nodes in the closed list
				{
					(*child_node).CalculateF(destination); // If it is NOT found, calculate its F and add it to the open list
					if (open.Find((*child_node).pos) != NULL) // If it is already in the open list, check if it is a better path (compare G)
					{
						if ((*open.Find((*child_node).pos)).g > (*child_node).g)
						{
							PathNode old_node = *open.Find((*child_node).pos); // If it is a better path, Update the parent
							old_node.parent = (*child_node).parent;
						}
					}
					else open.list.push_back(*child_node);
				}
				++child_node;
			}


		}
	

	return ret;
}
