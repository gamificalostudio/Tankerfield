#ifndef __M_SCENE_H__
#define __M_SCENE_H__

#define NEW_ROUND_PARTICLE_NUM 50

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
class UI_Image;

enum class GAME_STATE
{
	NO_TYPE,
	ENTER_IN_WAVE,
	IN_WAVE,
	EXIT_OF_WAVE,
	OUT_WAVE,
	GAME_OVER,
	WAITING_GAMEOVER,//Waiting for player input
	LEADER_BOARD,
	WAITING_LEADERBOARD//Waiting for player input
};

struct NewRoundUIParticles
{
	UI_Image * ui_image	= nullptr;
	fPoint direction	= { 0.f, 0.f };
	float curr_scale	=  1.f;
	uint curr_alpha		= 255;//TODO: Change to uint8
	bool reached_target = false;
};

class M_Scene : public Module
{
private:

	int number_of_enemies = 0;

public:

	SDL_Color tank_colors[4];

	int current_level				= 0;

	General_GUI * general_gui		= nullptr;

	uint round		= 0u;

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

	void CreateNewRoundParticles();

	// Called before all Updates
	bool PreUpdate() override;

	// Called each loop iteration
	bool Update(float dt) override;

	// Called before all Updates
	bool PostUpdate(float dt) override;

	bool Reset();

	// Called before quitting
	bool CleanUp() override;

	// ---------

	void DebugPathfinding();

	void ReduceNumEnemies();

private:

	//New round animation
	void PrepareNewRoundUIParticles();
	void UpdateNewRoundUIParticles(float dt);

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
	uint initial_num_enemies = 0u;
private:

	/* Game variables*/
	bool game_over = false;

	/* Wave System */

	uint Tesla_trooper_units			= 0u;
	uint Brute_units					= 0u;
	uint Suicidal_units					= 0u;
	uint RocketLauncher_units			= 0u;


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

	//New round animation
	NewRoundUIParticles new_round_ui_particles[NEW_ROUND_PARTICLE_NUM];
	float max_particle_scale = 0.f;
	float particle_speed = 0.f;
	float particle_speed_squared = 0.f;
};

#endif // __j1SCENE_H__