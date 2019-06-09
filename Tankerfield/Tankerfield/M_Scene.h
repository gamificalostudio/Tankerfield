#ifndef __M_SCENE_H__
#define __M_SCENE_H__

#include <vector>

#include "SDL/include/SDL_rect.h"

#include "Module.h"

#include "NewRoundAnimation.h"

#include "M_UI.h"

struct SDL_Texture;
struct Controller;

class Obj_Tank;
class RewardZone;
class Object;

class UI_Label;
class UI_Image;

class Player_GUI;
class General_GUI;
class Pause_Menu;
class LeaderBoard;
class Options_Menu;

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

class M_Scene : public Module
{
public:

	SDL_Color tank_colors[4];

	int current_level				= 0;

	General_GUI * general_gui		= nullptr;

	LeaderBoard * leaderboard		= nullptr;

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
	
	void SetMenuState(MENU_STATE new_state);

	void DebugPathfinding();


private:
	void CreateEnemyWave();

	void NewWave();


public:

	SDL_Texture* path_tex = nullptr;

private:
	/* Debug pathfinding */
	std::vector<iPoint> debug_path;
	bool test_path = true;

	iPoint path_tex_offset = { -30, 0 };
	uint initial_num_enemies = 0u;

private:
	//	Menus ---------------------------------

	MENU_STATE		menu_state = MENU_STATE::NO_TYPE;
	Pause_Menu*		pause_menu = nullptr;
	Options_Menu*	options_menu = nullptr;

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

	float time_between_waves = 0.f;//In seconds

private:
	/* Reward Zones */
	RewardZone* reward_zone_01 = nullptr;
	RewardZone* reward_zone_02 = nullptr;

	NewRoundAnimation new_round_animation;
};

#endif // __j1SCENE_H__