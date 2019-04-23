#ifndef __M_SCENE_H__
#define __M_SCENE_H__

#include <vector>

#include "SDL/include/SDL_rect.h"


#include "Module.h"

struct SDL_Texture;
struct Controller;

class Obj_Tank;
class RewardZone;
class Object;
class Player_GUI;
class General_HUD;

enum class WaveStat
{
	NO_TYPE,
	ENTER_IN_WAVE,
	IN_WAVE,
	EXIT_OF_WAVE,
	OUT_WAVE,
	GAME_OVER,
	WIN_GAME
};

class PerfTimer;

class M_Scene : public Module
{
private:
	Controller** control1			= nullptr;

public:

	int current_level				= 0;

	Obj_Tank * tank_1				= nullptr;
	Obj_Tank * tank_2				= nullptr;
	Obj_Tank * tank_3				= nullptr;
	Obj_Tank * tank_4				= nullptr;

	Player_GUI * player_1_gui       = nullptr;
	Player_GUI * player_2_gui       = nullptr;
	Player_GUI * player_3_gui       = nullptr;
	Player_GUI * player_4_gui       = nullptr;

	General_HUD * general_hud		= nullptr;

	uint round		= 0;
	std::list<Object*> enemies_in_wave;
	WaveStat stat_of_wave			= WaveStat::NO_TYPE;

	PerfTimer timer_between_waves;

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

	bool Reset();

	// Called before quitting
	bool CleanUp() override;

	bool draw_debug = false;
	// ---------

	void DebugPathfinding();

private:
	void CreateEnemyWave();

	void NewWave();

	bool AllPlayersReady() const;

public:
	SDL_Texture* path_tex = nullptr;

private:
	/* Debug pathfinding */
	std::vector<iPoint> debug_path;
	bool test_path = true;

	iPoint path_tex_offset = { -30, 0 };
	uint initial_num_enemies = 0;
private:

	/* Game variables*/
	bool game_over = false;
	bool win_game = false;

	int rounds_to_win = NULL;
	int current_wave = 0;
	float accumulated_time = 0.0f;
	// Every two seconds we check
	float time_round_check_frequency = 0.0f;
	bool perform_round_check = false;

	/* Wave System */
	int time_between_rounds			= 0;
	int Tesla_trooper_units			= 0;
	int Brute_units					= 0;

	// Fx and Music
	const char* finish_wave_sound_string;
	uint finish_wave_sound_uint = 0u; 
	int finish_wave_sound_channel = -1;
	
	const char* wind_sound_string;
	uint wind_sound_uint = 0u;
	int wind_sound_channel = -1;

	const char* main_music;


private:
	/* Reward Zones */
	RewardZone* reward_zone_01 = nullptr;
	RewardZone* reward_zone_02 = nullptr;

};

#endif // __j1SCENE_H__