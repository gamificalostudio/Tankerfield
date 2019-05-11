#include "Brofiler\Brofiler.h"

#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "M_input.h"
#include "M_render.h"
#include "M_Textures.h"
#include "M_map.h"
#include "M_Pathfinding.h"

M_JumpPointSearch::M_JumpPointSearch() : Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.assign("JumpPointSearch_pathfinding");
}

// Destructor
M_JumpPointSearch::~M_JumpPointSearch()
{
	RELEASE_ARRAY(map);
}

bool M_JumpPointSearch::Start()
{
	path_tex = app->tex->Load(app->config.child("pathfinding").child("debug_textures").child_value());
	path_tex_offset = {-30,0};
	return true;
}

bool M_JumpPointSearch::PostUpdate(float dt)
{
	DebugPathfinding();
	return true;
}


// Called before quitting
bool M_JumpPointSearch::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.clear();

	RELEASE_ARRAY(map);

	return true;
}

bool M_JumpPointSearch::Reset()
{
	last_path.clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void M_JumpPointSearch::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = data;
}

// Utility: return true if pos is inside the map boundaries
bool M_JumpPointSearch::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool M_JumpPointSearch::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar M_JumpPointSearch::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

void M_JumpPointSearch::DebugPathfinding()
{
		std::vector<Camera*>::iterator item_cam= app->render->cameras.begin();
		if (item_cam == app->render->cameras.end())
			return;

		static iPoint origin;
		static bool origin_selected = false;
		static bool createdDebugPath = false;

		iPoint mousePos;
		app->input->GetMousePosition(mousePos.x, mousePos.y);
		iPoint p = app->render->ScreenToWorld(mousePos.x, mousePos.y, *item_cam);
		p = app->map->ScreenToMapI(p.x, p.y);

		if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN)
		{
			if (origin_selected == true)
			{
				origin_selected = false;

				if (app->pathfinding->CreatePath(origin, p) != -1)
				{
					createdDebugPath = true;
				}

			}
			else
			{
				origin = p;
				origin_selected = true;
				createdDebugPath = false;
				debug_path.clear();
			}
		}

		if (createdDebugPath)
		{
			uint debugPathSize = debug_path.size();
			if (debugPathSize == 0)
			{

				const std::vector<iPoint>* path = app->pathfinding->GetLastPath();
				uint sizeArray = path->size();
				for (uint i = 0; i < sizeArray; ++i)
				{
					debug_path.push_back(path->at(i));
				}
			}
			else
			{
				for (uint i = 0; i < debugPathSize; ++i)
				{
					iPoint pos = app->map->MapToScreenI(debug_path.at(i).x, debug_path.at(i).y);

					for (item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); ++item_cam)
					{
						SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->screen_section);
						app->render->Blit(path_tex, pos.x + path_tex_offset.x, pos.y + path_tex_offset.y, (*item_cam));
					}
					SDL_RenderSetClipRect(app->render->renderer, nullptr);
				}
			}

		}

		p = app->map->MapToScreenI(p.x, p.y);

		for (item_cam = app->render->cameras.begin(); item_cam != app->render->cameras.end(); ++item_cam)
		{
			SDL_RenderSetClipRect(app->render->renderer, &(*item_cam)->screen_section);
			app->render->Blit(path_tex, p.x + path_tex_offset.x, p.y + path_tex_offset.y, (*item_cam));
		}SDL_RenderSetClipRect(app->render->renderer, nullptr);

}

void M_JumpPointSearch::Search_horizontal(const PathNode & start_node, const PathNode & self_node, int hor_dir, int dist, const PathList& list_to_fill)
{

}


// To request all tiles involved in the last generated path
const std::vector<iPoint>* M_JumpPointSearch::GetLastPath() const
{
	return &last_path;
}




// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int M_JumpPointSearch::CreatePath(const iPoint& origin, const iPoint& destination)
{
	BROFILER_CATEGORY("CRATE PATH", Profiler::Color::LightBlue);
	last_path.clear();

	int ret = -1;

	if (origin == destination || !IsWalkable(origin) || !IsWalkable(destination))
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
