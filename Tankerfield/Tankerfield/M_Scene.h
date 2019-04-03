#ifndef __M_SCENE_H__
#define __M_SCENE_H__

#include <vector>

#include "Module.h"
#include "SDL/include/SDL_rect.h"

struct SDL_Texture;
struct Controller;
class Obj_Tank;

class M_Scene : public Module
{
private:
	Controller** control1 = nullptr;


	


public:
	int current_level = 0;
	Obj_Tank * tank_1 = nullptr;
	Obj_Tank * tank_2 = nullptr;
public:

	M_Scene();

	// Destructor
	virtual ~M_Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	bool Start() override;

	// Called before all Updates
	bool PreUpdate() override;

	// Called each loop iteration
	bool Update(float dt) override;

	// Called before all Updates
	bool PostUpdate(float dt) override;

	// Called before quitting
	bool CleanUp() override;

	// ---------

	void DebugPathfinding();

private:
	/* Debug pathfinding */
	std::vector<iPoint> debugPath;
	bool test_path = true;
	SDL_Texture* path_tex = nullptr;

	iPoint path_tex_offset = { 0, 16 };
};

#endif // __j1SCENE_H__