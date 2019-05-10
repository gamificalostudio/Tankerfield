#ifndef M_JUMPPOINTSEARCH_H_
#define M_JUMPPOINTSEARCH_H_

#include <vector>
#include <list>
#include <algorithm>
#include "Module.h"
#include "Point.h"
#include "M_Textures.h"
#include "M_Pathfinding.h"
#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

struct SDL_Texture;

class M_JumpPointSearch: public Module
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

	SDL_Texture* path_texture = nullptr;
};




#endif // !M_JUMPPOINTSEARCH_H_

