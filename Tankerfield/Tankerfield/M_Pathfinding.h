#ifndef __M_PATHFINDING_H__
#define __M_PATHFINDING_H__

#include <vector>
#include <list>
#include <algorithm>

#include "Module.h"
#include "Point.h"
#include "M_Textures.h"


// forward declaration
struct PathList;

// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill) const;

	//start_node-> start position of the horizontal scan
	//hor_dir -> Horizontal direction (+1 or -1). 
	// dist -> Distance traveled so far. 
	// return jumpPoints
	bool Search_horizontal(int hor_dir, int dist, PathList& list_to_fill, const iPoint& goal);

	bool Search_vertical(int hor_dir, int dist, PathList& list_to_fill, const iPoint& goal);

	bool Search_diagonal(int hor_dir, int vert_dir, PathList & list_to_fill, iPoint goal);
	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	// -----------
	int g = NULL;
	int h = NULL;
	iPoint pos;
	const PathNode* parent; // needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	const PathNode* Find(const iPoint& point) const;

	// Returns the Pathnode with lowest score in this list or NULL if empty
	const PathNode* GetNodeLowestScore() const;

	// -----------
	// The list itself, note they are not pointers!
	std::list<PathNode> list;
};


#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

struct SDL_Texture;

enum class DIRECTIONS
{
	NORTH,
	EAST,
	SOUTH,
	WEST,
	NORTH_EAST,
	SOUTH_EAST,
	WEST_SOUTH,
	WEST_NORTH
};

class M_Pathfinding : public Module
{
public:
	
	M_Pathfinding();

	// Destructor
	~M_Pathfinding();

	// Called before quitting
	bool CleanUp() override;

	bool Reset();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Main function to request a path from A to B
	int CreatePath(const iPoint& origin, const iPoint& destination);

	// To request all tiles involved in the last generated path
	const std::vector<iPoint>* GetLastPath() const;

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	bool IsWalkable(int x, int y) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const iPoint& pos) const;

	
private:

	// size of the map
	uint width = NULL;
	uint height = NULL;
	// all map walkability values [0..255]
	uchar* map = nullptr;
	// we store the created path here
	std::vector<iPoint> last_path;

	friend class PathNode;
};


class M_JumpPointSearch : public Module
{
public:

	M_JumpPointSearch();

	// Destructor
	~M_JumpPointSearch();

	bool Start() override;
	//Here were going to DebugPathfinding
	bool PostUpdate(float dt) override;

	// Called before quitting
	bool CleanUp() override;

	bool Reset();

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Main function to request a path from A to B
	int CreatePath(const iPoint& origin, const iPoint& destination);

	// To request all tiles involved in the last generated path
	const std::vector<iPoint>* GetLastPath() const;

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const iPoint& pos) const;

	void DebugPathfinding();


	

private:

	// size of the map
	uint width = NULL;
	uint height = NULL;
	// all map walkability values [0..255]
	uchar* map = nullptr;
	// we store the created path here
	std::vector<iPoint> last_path;

	std::vector<iPoint> debug_path;

	SDL_Texture* path_tex = nullptr;
	iPoint path_tex_offset;
};



#endif // __j1PATHFINDING_H__
