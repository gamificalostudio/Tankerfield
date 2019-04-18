#ifndef __M_SCENE_H__
#define __M_SCENE_H__

#include <vector>

#include "SDL/include/SDL_rect.h"

#include "Module.h"

struct SDL_Texture;
struct Controller;
class Obj_Tank;
class RewardZone;

class M_Scene : public Module
{
private:
	Controller** control1 = nullptr;

public:
	int current_level = 0;
	Obj_Tank * tank_1 = nullptr;
	Obj_Tank * tank_2 = nullptr;
	Obj_Tank * tank_3 = nullptr;
	Obj_Tank * tank_4 = nullptr;
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

	bool draw_debug = false;
	// ---------

	void DebugPathfinding();

private:
	void CreateEnemyWave();

public:
	SDL_Texture* path_tex = nullptr;

private:
	/* Debug pathfinding */
	std::vector<iPoint> debug_path;
	bool test_path = true;

	iPoint path_tex_offset = { -30, 0 };
	uint initial_num_enemies = 0;
private:
	/* Wave System */
	// In milliseconds
	float accumulated_time = 0.0f;
	bool perform_objects_check = false;

	int time_between_rounds = 0;
	int generated_units = 0;
	int distance_range = 0;
	int min_distance_from_center = 0;
	int check_complete_round = 0;
	int object_counter = 0;
	int enemies_to_increase = 0;

private:
	/* Reward Zones */
	RewardZone* reward_zone_01 = nullptr;
	RewardZone* reward_zone_02 = nullptr;
};

#endif // __j1SCENE_H__