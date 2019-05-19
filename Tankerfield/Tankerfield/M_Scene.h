#ifndef __M_SCENE_H__
#define __M_SCENE_H__

#define MAX_SUBROUNDS 3

#include <vector>

#include "SDL/include/SDL_rect.h"


#include "Module.h"

struct SDL_Texture;
struct Controller;

class Obj_Tank;
class RewardZone;
class Object;
class Player_GUI;
class General_GUI;
class UI_Label;


enum class GAME_STATE
{
	NO_TYPE,
	ENTER_IN_WAVE,
	IN_WAVE,
	EXIT_OF_WAVE,
	OUT_WAVE,
	GAME_OVER,
	GAME_WON,
	WAIT_PLAYER_INPUT_1,
	LEADER_BOARD,
	WAIT_PLAYER_INPUT_2
};

class PerfTimer;

class M_Scene : public Module
{
private:

	int number_of_enemies = 0;

public:

	SDL_Color tank_colors[4];

	int current_level				= 0;

	General_GUI * general_gui		= nullptr;

	uint round		= 0;

	GAME_STATE game_state			= GAME_STATE::NO_TYPE;

	Timer timer_between_waves;

	UI_Label* label_number_of_enemies = nullptr;

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

	void ReduceNumEnemies();

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
	float accumulated_time = 0.0f;

	// Every two seconds we check
	float time_round_check_frequency = 0.0f;
	bool perform_round_check = false;

	/* Wave System */
	int time_between_rounds[MAX_SUBROUNDS] = { 0,0,0 };

	uint Tesla_trooper_units			= 0;

	uint Brute_units					= 0;

	float percentage_enemies_subround[MAX_SUBROUNDS] = { 0.f,0.f,0.f };

	float wait_time[MAX_SUBROUNDS] = { 0.f,0.f,0.f };

	uint subround = 0;


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